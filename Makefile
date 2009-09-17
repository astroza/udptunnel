INCLUDE = -I./include
CC = gcc
#CC = /home/felipe/Sources/OpenWrt-SDK-brcm47xx-2.6-for-Linux-i686/staging_dir_mipsel/bin/mipsel-linux-gcc
CFLAGS = -Wall

all: socket.o tun.o util.o utunclient.o utunserver.o
	$(CC) socket.o tun.o util.o utunclient.o -o utunclient
	$(CC) socket.o tun.o util.o utunserver.o -o utunserver

socket.o: socket.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $?

tun.o: tun.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $?

util.o: util.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $?

utunclient.o: utunclient.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $?

utunserver.o: utunserver.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $?

clean:
	rm -f *.o utunclient utunserver
