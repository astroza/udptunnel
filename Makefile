INCLUDE = -I./include
CC = gcc
#CC = /home/felipe/Sources/OpenWrt-SDK-brcm47xx-2.6-for-Linux-i686/staging_dir_mipsel/bin/mipsel-linux-gcc
CFLAGS = -Wall

all: socket.o tun.o util.o client.o server.o
	$(CC) socket.o tun.o util.o client.o -o client
	$(CC) socket.o tun.o util.o server.o -o server

socket.o: socket.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $?

tun.o: tun.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $?

util.o: util.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $?

client.o: client.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $?

server.o: server.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $?

clean:
	rm -f *.o client server
