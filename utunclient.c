/* UDP Tunnel - utunclient.c
 * 2009 - Felipe Astroza
 * Under GPLv2 license (see LICENSE)
 */

#include <stdio.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <utun/socket.h>
#include <utun/tun.h>
#include <utun/util.h>
#include <utun/conf.h>

#define PASSPHRASE v[3]

int main(int c, char **v)
{
	struct pollfd pfd[2];
	struct sockaddr_in serv_addr, from;
	socklen_t fromlen;
	char buf[MTU];
	unsigned int buflen;
	int ret;

	if(c < 4) {
		printf( "UDP Tunnel\n"
			"%s <ip address> <port> <passphrase>\n", v[0]);
		return 0;
	}

	serv_addr.sin_addr.s_addr = inet_addr(v[1]);
	if(strtoport(v[2], &serv_addr.sin_port) == 0) {
		printf("%s: Invalid port\n", v[0]);
		return 1;
	}
	serv_addr.sin_family = AF_INET;

	pfd[0].fd = tun_create("utun%d");
	pfd[0].events = POLLIN;


	pfd[1].fd = socket_create(0);
	pfd[1].events = POLLIN;

	sendto(pfd[1].fd, PASSPHRASE, strlen(PASSPHRASE), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	puts("UDP TUNNEL Running..");
	while(1) {
		ret = poll(pfd, 2, -1);

		if(ret == -1)
			break;

		if(pfd[0].revents & POLLIN) {
			buflen = tun_get_packet(pfd[0].fd, buf, sizeof(buf));
			socket_put_packet(pfd[1].fd, &serv_addr, sizeof(serv_addr),  buf, buflen);
		}

		if(pfd[1].revents & POLLIN) {
			buflen = socket_get_packet(pfd[1].fd, &from, &fromlen, buf, sizeof(buf));

			if(serv_addr.sin_addr.s_addr == from.sin_addr.s_addr && serv_addr.sin_port == from.sin_port)
				tun_put_packet(pfd[0].fd, buf, buflen);
		}
	}

	return 1;
}
