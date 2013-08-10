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
