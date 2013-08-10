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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <net/if.h>
#ifdef __linux__ 
#include <linux/if_tun.h>
#else
#include <net/if_utun.h>
#endif
#include <sys/ioctl.h>
#include <unistd.h>

#ifdef __linux__
static const char device_name[] = "/dev/net/tun";
#else
static const char device_name[] = "/dev/tun0";
#endif

int tun_create()
{
	int fd;

	if ((fd = open(device_name,O_RDWR)) == -1) {
		perror("open");
		exit(1);
	}
#ifdef __linux__
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN|IFF_NO_PI;
	strncpy(ifr.ifr_name, "tun0", IFNAMSIZ);
	if(ioctl(fd, TUNSETIFF, (void *)&ifr) == -1) {
		perror("perror");
		exit(1);
	}
#endif
	return fd;
}

unsigned int tun_get_packet(int fd, char *buf, unsigned int bufsize)
{
	return read(fd, buf, bufsize);
}

void tun_put_packet(int fd, char *buf, unsigned int buflen)
{
	if(write(fd, buf, buflen) == -1) {
		perror("write");
		exit(1);
	}
}
