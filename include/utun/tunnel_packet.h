#ifndef TUNNEL_PACKET_H
#define TUNNEL_PACKET_H

#define MTU 		1500

#define CONTROL_PACKET 	0
#define TRAFFIC_PACKET 	1

#define AUTH_CMD 	0
#define OK_CMD 		1
#define ERROR_CMD	2

struct tunnel_packet {
	unsigned char type;
	unsigned char cmd;
	char data[];
};

#endif
