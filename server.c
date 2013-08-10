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

#include <utun/socket.h>
#include <utun/tun.h>
#include <utun/util.h>
#include <utun/tunnel_packet.h>

#define PASSPHRASE v[2]

int main(int c, char **v)
{
	struct sockaddr_in client_addr, from;
	socklen_t fromlen;
	fd_set rfds;
	struct tunnel_packet *tp;
	char buf[MTU];
	unsigned int buflen;
	unsigned short local_port;
	int ret;
	int tun_fd;
	int client_fd;
	int has_client = 0;
	unsigned int pass_len;

	tp = (struct tunnel_packet *)buf;
	
	if(c < 3) {
		printf( "UDP Tunnel\n"
			"%s <port> <passphrase>\n", v[0]);
		return 0;
	}

	if(strtoport(v[1], &local_port) == 0) {
		printf("%s: Invalid port\n", v[0]);
		return 1;
	}
	
	pass_len = strlen(PASSPHRASE);
	tun_fd = tun_create();
	client_fd = socket_create(local_port);
	fromlen = sizeof(from);

#ifdef __linux__
        exec_script("linux_server.sh", v[1]);
#else
        exec_script("osx_server.sh", v[1]);
#endif
	
	FD_ZERO(&rfds);
	while(1) {
		FD_SET(client_fd, &rfds);
		FD_SET(tun_fd, &rfds);
		ret = select(client_fd+1, &rfds, NULL, NULL, NULL);

		if(ret == -1)
			break;

		if(FD_ISSET(tun_fd, &rfds)) {
			buflen = tun_get_packet(tun_fd, tp->data, sizeof(buf)-sizeof(struct tunnel_packet));
			tp->type = TRAFFIC_PACKET;
			tp->cmd = 0;
			if(has_client)
				socket_put_packet(client_fd, &client_addr, sizeof(client_addr), buf, buflen + sizeof(struct tunnel_packet));
		}

		if(FD_ISSET(client_fd, &rfds)) {
			buflen = socket_get_packet(client_fd, &from, &fromlen, buf, sizeof(buf));
			if(tp->type == TRAFFIC_PACKET) {
				if(client_addr.sin_addr.s_addr == from.sin_addr.s_addr && client_addr.sin_port == from.sin_port)
					tun_put_packet(tun_fd, tp->data, buflen-sizeof(struct tunnel_packet));
			} else if(tp->type == CONTROL_PACKET && tp->cmd == AUTH_CMD) {
				if(buflen-sizeof(struct tunnel_packet) == pass_len && strncmp(tp->data, PASSPHRASE, pass_len) == 0) {
					client_addr = from;
					tp->cmd = OK_CMD;
					has_client = 1;
				} else
					tp->cmd = ERROR_CMD;
				socket_put_packet(client_fd, &from, sizeof(from), buf,  sizeof(struct tunnel_packet));
			}
		}
	}

	return 1;
}
