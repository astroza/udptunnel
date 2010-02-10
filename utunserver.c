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

#define MTU 1500
#define PASSPHRASE v[2]

int main(int c, char **v)
{
	struct pollfd pfd[2];
	struct sockaddr_in client_addr, from;
	socklen_t fromlen;
	char buf[MTU];
	unsigned int buflen;
	unsigned short local_port;
	int ret;

	if(c < 3) {
		printf( "UDP Tunnel\n"
			"%s <port> <passphrase>\n", v[0]);
		return 0;
	}

	if(strtoport(v[1], &local_port) == 0) {
		printf("%s: Invalid port\n", v[0]);
		return 1;
	}
	
	pfd[0].fd = tun_create("utun%d");
	pfd[0].events = POLLIN;

	pfd[1].fd = socket_create(local_port);
	pfd[1].events = POLLIN;

	fromlen = sizeof(from);

	while(1) {
		ret = poll(pfd, 2, -1);

		if(ret == -1)
			break;

		if(pfd[0].revents & POLLIN) {
			buflen = tun_get_packet(pfd[0].fd, buf, sizeof(buf));
			socket_put_packet(pfd[1].fd, &client_addr, fromlen, buf, buflen);
		}

		if(pfd[1].revents & POLLIN) {
			buflen = socket_get_packet(pfd[1].fd, &from, &fromlen, buf, sizeof(buf));

			if(client_addr.sin_addr.s_addr != from.sin_addr.s_addr || client_addr.sin_port != from.sin_port) {
				if(strncmp(PASSPHRASE, buf, strlen(PASSPHRASE)) == 0) {
					memcpy(&client_addr, &from, sizeof(struct sockaddr_in));
					printf("%s: New client\n", v[0]);
				}
			} else
				tun_put_packet(pfd[0].fd, buf, buflen);
		}
	}

	return 1;
}
