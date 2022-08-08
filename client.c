/*
 * udptunnel
 * Copyright © 2013 Felipe Astroza A.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <utun/socket.h>
#include <utun/tun.h>
#include <utun/util.h>
#include <utun/tunnel_packet.h>
#include <unistd.h>
#include "log.h"
#include "ini.h"

int main(int argc, char *argv[])
{
	struct sockaddr_in server_addr, from;
	socklen_t fromlen;
	fd_set rfds;
	struct tunnel_packet *tp;
	char buf[MTU];
	unsigned int buflen;
	int ret;
	int tun_fd;
	int server_fd;
	char *ini_file=NULL;
	char *server_address=NULL;
	char *server_port=NULL;
	char *auth=NULL;
	char *outbound_key = NULL;
	char *inbound_key = NULL;
	char *outbound_counter = NULL;
	char *inbound_counter = NULL;
	
	int log_level=LOG_INFO;
	int c=0;
	tp = (struct tunnel_packet *)buf;

	/* ini-file */
	while ((c = getopt (argc, argv, "dhi:")) != -1)
	switch (c)
	{
		case 'd':
			log_level=LOG_DEBUG;
			break;
		case 'i':
			ini_file = optarg;
			break;
		case 'h':
			log_info("[%d] client",getpid());
			log_info("[%d] Usage: -i [ini_file] ",getpid());
			log_info("[%d]        -d debug log ",getpid());
			return 1;
		break;
			default:
			break;
	}
	if (ini_file == NULL) 
	{
		log_error("[%d] ini file not specified, exiting.", getpid());
		return 0;
	}
	
	log_set_level(log_level);
	ini_t *config = ini_load(ini_file);
	if (config == NULL ) {
		log_error("[%d] Cannot open ini-file, exiting.", getpid());
		return 0;
	}
	ini_sget(config, "client", "server_address", NULL, &server_address);
	ini_sget(config, "client", "server_port", NULL, &server_port);
	ini_sget(config, "client", "auth", NULL, &auth);
	ini_sget(config, "client", "outbound_key", NULL, &outbound_key);
	ini_sget(config, "client", "inbound_key", NULL, &inbound_key);
	ini_sget(config, "client", "outbound_counter_file", NULL, &outbound_counter);
	ini_sget(config, "client", "inbound_counter_file", NULL, &inbound_counter);
	log_info("[%d] Server address: %s ",getpid(),server_address);
	log_info("[%d] Server port %s",getpid(),server_port);
	log_info("[%d] Auth: %s",getpid(),auth);

	if( access( outbound_key, W_OK ) == 0 ) {
		log_info("[%d] Outbound key file: %s",getpid(),outbound_key);
	} else {
		log_error("[%d] Cannot open outbound key file: %s. Need a writable file.",getpid(),outbound_key);
	}
	if( access( inbound_key, W_OK ) == 0 ) {
		log_info("[%d] Inbound key file: %s",getpid(),inbound_key);
	} else {
		log_error("[%d] Cannot open inbound key file: %s. Need a writable file.",getpid(),inbound_key);
	}
	if( access( outbound_counter, W_OK ) == 0 ) {
		log_info("[%d] Outbound key counter file: %s",getpid(),outbound_counter);
	} else {
		log_error("[%d] Cannot open outbound key counter file: %s. Need a writable file.",getpid(),outbound_counter);
	}
	if( access( inbound_counter, W_OK ) == 0 ) {
		log_info("[%d] Inbound key counter file: %s",getpid(),inbound_counter);
	} else {
		log_error("[%d] Cannot open inbound key counter file: %s. Need a writable file.",getpid(),inbound_counter);
	}
	
	struct hostent* host_info = gethostbyname(server_address);
	if (host_info == NULL) {
		log_error("[%d] Invalid hostname or IPv4 address: %s",getpid(),server_address);
		return 1;
	}
	log_info("[%d] Client starting",getpid());
	memset(&server_addr, 0, sizeof(server_addr));
	memset(&from, 0, sizeof(from));
	memcpy(&server_addr.sin_addr, host_info->h_addr, host_info->h_length);
	if(strtoport(server_port, &server_addr.sin_port) == 0) {
		log_error("[%d] Invalid port: %s",getpid(),server_port);
		return 1;
	}
	server_addr.sin_family = AF_INET;
	tun_fd = tun_create();
	server_fd = socket_create(0);	
	log_info("[%d] Auth bytes disabled",getpid());
	exec_script("linux_client.sh", server_address);
	log_info("[%d] Tunnel is running",getpid());
	FD_ZERO(&rfds);
	
	while(1) {
		FD_SET(server_fd, &rfds);
		FD_SET(tun_fd, &rfds);
		ret = select(server_fd+1, &rfds, NULL, NULL, NULL);
		if(ret == -1)
			break;
		if(FD_ISSET(tun_fd, &rfds)) {
			buflen = tun_get_packet(tun_fd, tp->data, sizeof(buf)-sizeof(struct tunnel_packet));
			tp->type = TRAFFIC_PACKET;
			tp->cmd = 0;
			socket_put_packet(server_fd, &server_addr, sizeof(server_addr), buf, buflen + sizeof(struct tunnel_packet),outbound_key,outbound_counter);
		}
		if(FD_ISSET(server_fd, &rfds)) {
			buflen = socket_get_packet(server_fd, &from, &fromlen, buf, sizeof(buf),inbound_key,inbound_counter);
			if(server_addr.sin_addr.s_addr == from.sin_addr.s_addr && server_addr.sin_port == from.sin_port)
				tun_put_packet(tun_fd, tp->data, buflen-sizeof(struct tunnel_packet));
		}
	}
	return 1;
}
