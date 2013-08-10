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
