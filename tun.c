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
#include <string.h>
#ifdef __linux__ 
#include <linux/if_tun.h>
#else
#include <net/if_utun.h>
#include <sys/sys_domain.h>
#include <sys/kern_control.h>
#endif
#include <sys/ioctl.h>
#include <unistd.h>

int tun_create()
{
	int fd;

#ifdef __linux__
        if ((fd = open("/dev/net/tun", O_RDWR)) == -1) {
                perror("open");
                exit(1);
	}
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN;
	strncpy(ifr.ifr_name, "tun0", IFNAMSIZ);
	if(ioctl(fd, TUNSETIFF, (void *)&ifr) == -1) {
		perror("perror");
		exit(1);
	}
#else
	struct sockaddr_ctl sc;
	struct ctl_info ctlInfo;

	fd = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);
	if(fd < 0) { 
		perror ("socket");
		exit(1);
	}
	memset(&ctlInfo, 0, sizeof(ctlInfo));
	strncpy(ctlInfo.ctl_name, UTUN_CONTROL_NAME, sizeof(ctlInfo.ctl_name));
	if (ioctl(fd, CTLIOCGINFO, &ctlInfo) == -1) {
		perror("CTLIOCGINFO");
		exit(1);
	}
	sc.sc_family = PF_SYSTEM;
	sc.ss_sysaddr = AF_SYS_CONTROL;
	sc.sc_id = ctlInfo.ctl_id;
	sc.sc_len = sizeof(sc);
	sc.sc_unit = 0;
	if(connect(fd, (struct sockaddr *)&sc, sizeof(sc)) == -1) {
		perror("connect");
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
