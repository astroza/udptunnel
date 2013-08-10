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
