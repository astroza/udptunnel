/* UDP Tunnel - socket.h
 * 2009 - Felipe Astroza
 * Under GPLv2 license (see LICENSE)
 */

#ifndef SOCKET_H
#define SOCKET_H

int socket_create(unsigned short port);
void socket_put_packet(int fd, struct sockaddr_in *sa, socklen_t salen, char *buf, unsigned int buflen);
unsigned int socket_get_packet(int fd, struct sockaddr_in *sa, socklen_t *salen, char *buf, unsigned int bufsize);

#endif
