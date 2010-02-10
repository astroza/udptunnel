#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <net/if.h> 
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <unistd.h>

int tun_create(const char *name)
{
	struct ifreq ifr;
	int fd;

	if ((fd = open("/dev/net/tun",O_RDWR)) == -1) {
		perror("open");
		exit(1);
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN;
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	if(ioctl(fd, TUNSETIFF, (void *)&ifr) == -1) {
		perror("perror");
		exit(1);
	}

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
