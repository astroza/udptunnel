/* UDP Tunnel - socket.c
 * 2009 - Felipe Astroza
 * Under GPLv2 license (see LICENSE)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int socket_create(unsigned short port)
{
	struct sockaddr_in sa;

	int fd = socket(AF_INET, SOCK_DGRAM, 0);

	if(fd == -1) {
		perror("socket");
		exit(1);
	}

	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = port;
	sa.sin_family = AF_INET;

	if(bind(fd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
		perror("bind");
		exit(1);
	}

	return fd;
}

void socket_put_packet(int fd, struct sockaddr_in *sa, socklen_t salen, char *buf, unsigned int buflen)
{
	if(sendto(fd, buf, buflen, 0, (struct sockaddr *)sa, salen) == -1) {
		perror("sendto");
		exit(1);
	}
}

unsigned int socket_get_packet(int fd, struct sockaddr_in *sa, socklen_t *salen, char *buf, unsigned int bufsize)
{
	return recvfrom(fd, buf, bufsize, 0, (struct sockaddr *)sa, salen);
}
