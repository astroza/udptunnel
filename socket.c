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
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "log.h"
#include "binn.h"

void safe_fclose(FILE *fp);
long int get_key_index(char *filename);
void set_key_index(char *filename, long int index);

void update_fifo(float key_presentage)
{
	int fd;
    char *key_status_fifo = "/tmp/tx-key-presentage";
    mkfifo(key_status_fifo, 0666);
    char presentage_string[10];
    memset(presentage_string,0,10);
    sprintf(presentage_string,"%.2f %%",key_presentage);
	fd = open(key_status_fifo, O_WRONLY| O_NONBLOCK);
	write(fd, presentage_string, strlen(presentage_string)+1);
	close(fd);
}

void update_rx_fifo(float key_presentage)
{
	int fd;
    char *key_status_fifo = "/tmp/rx-key-presentage";
    mkfifo(key_status_fifo, 0666);
    char presentage_string[10];
    memset(presentage_string,0,10);
    sprintf(presentage_string,"%.2f %%",key_presentage);
	fd = open(key_status_fifo, O_WRONLY| O_NONBLOCK);
	write(fd, presentage_string, strlen(presentage_string)+1);
	close(fd);
}

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

void safe_fclose(FILE *fp)
{
	if (fp && fp != stdout && fp != stderr) {
		if (fclose(fp) == EOF) {
			perror("fclose()");
		}
		fp = NULL;
	}
}

long int get_file_size (char *filename) {
	struct stat st;
	long int size=0;
	stat(filename, &st);
	size = st.st_size;
	return size;
}

long int get_key_index(char *filename) {
	long int index=0;
	FILE *keyindex_file;
	keyindex_file = fopen(filename, "rb");
	fread(&index, sizeof(long int),1,keyindex_file);
	safe_fclose(keyindex_file);
	return index;
}

void set_key_index(char *filename, long int index) {
	FILE *keyindex_file;
	keyindex_file = fopen(filename, "wb");
	fwrite(&index, sizeof(long int), 1, keyindex_file);
	safe_fclose(keyindex_file);
}

void getkey(char *filename, char* keybuf, long int start_index, int len, bool overwrite)
{	
	FILE *keyfile;
	size_t freadlen=0;
	keyfile = fopen(filename, "rb");
	if (fseek(keyfile, start_index, SEEK_SET)) {
			printf("Seek error!\n");
	}
	freadlen = fread(keybuf, sizeof(char),len,keyfile);
	if ( freadlen == 0 ) {
		log_error("[%d] %s fread return: %d ", getpid(),filename,freadlen);	
		log_error("[%d] You run out of key material! Exiting. ", getpid());	
		exit(0);
	}
	safe_fclose(keyfile);
	if ( overwrite == TRUE )
	{
		log_debug("[%d] Key %s overwrite at: %ld len: %d", getpid(),filename,start_index,len);
		char *zerobuf = malloc(len);
		memset(zerobuf,0xFF,len);
		int f_read = open(filename, O_WRONLY);
		lseek (f_read, start_index, SEEK_CUR);
		write(f_read, zerobuf, len);
		close(f_read);
		free(zerobuf);
		log_debug("[%d] Key overwrite complete and buffers free'd", getpid() );
	}
}

void print_hex_memory(void *mem, int buflen) {
  int i;
  unsigned char *p = (unsigned char *)mem;
  for (i=0;i<buflen;i++) {
    printf("0x%02x ", p[i]);
    if ((i%16==0) && i)
      printf("\n");
  }
  printf("\n");
}

int encryptpacket(char *buf, unsigned int buflen, char *serializedbuf, char *keyfile,char* outbound_counter_file)
{	
	static long int tx_key_ref;
	int packet_size;
	unsigned char *xorbytes = malloc(buflen);
	memset(xorbytes, 0, buflen);
	char *key = malloc(buflen);
	memset(key, 0, buflen);
	long int tx_key_used = get_key_index(outbound_counter_file);
	
		getkey(keyfile,key,tx_key_used-buflen,buflen,TRUE);
		for(int i = 0; i < buflen; ++i)
		{
			xorbytes[i] = buf[i] ^ key[i];
		}

	tx_key_used = tx_key_used + buflen;
	set_key_index(outbound_counter_file, tx_key_used);
		
		binn *obj;
		obj = binn_object();
		binn_object_set_blob(obj, "packet", xorbytes,buflen);
		binn_object_set_int64(obj, "keyindex", tx_key_used-buflen);
		binn_object_set_int32(obj, "buflen", buflen);	
		memcpy(serializedbuf,binn_ptr(obj), binn_size(obj));
		packet_size = binn_size(obj);
		binn_free(obj);
		free(xorbytes);
		
		if (tx_key_used > tx_key_ref ) {
			long int key_file_size = get_file_size(keyfile);
			float key_presentage = (100.0*tx_key_used)/key_file_size;
			tx_key_ref = tx_key_used + 500000;
			log_info("[%d] TX key used: %ld (of %ld) %.2f %%",getpid(),tx_key_used,key_file_size,key_presentage );
			update_fifo(key_presentage);
		}
	 return packet_size;
}

void socket_put_packet(int fd, struct sockaddr_in *sa, socklen_t salen, char *buf, unsigned int buflen, char *keyfile,char* outbound_counter_file)
{
	char serializedbuf[2000]; 
	memset(serializedbuf, 0, 2000);
	int serialized_packet_len = encryptpacket(buf,buflen,serializedbuf,keyfile, outbound_counter_file);
	if( sendto( fd, serializedbuf, serialized_packet_len, 0, (struct sockaddr *)sa, salen ) == -1) {
		perror("sendto");
		exit(1);
	}
}

int decryptpacket(char *buf,char *rxbuffer,int readbytes,char* keyfile, char* inbound_counter_file)
{	
	static long int rx_key_ref;
	unsigned char *serializedbuf; 
	long int keyindex;
	int buflen;
	char *key;
	binn *obj;
	obj = binn_open(rxbuffer);
	keyindex = binn_object_int64(obj, "keyindex");
	buflen = binn_object_int32(obj, "buflen");
	serializedbuf = binn_object_blob(obj, "packet",&buflen);
	binn_free(obj);
	key = malloc(buflen);
	memset(key, 0, buflen);
	getkey(keyfile,key,keyindex-buflen,buflen,TRUE);	
	for(int i = 0; i < buflen; ++i)
	{
		buf[i] = serializedbuf[i] ^ key[i];
	}
	set_key_index(inbound_counter_file, keyindex + buflen );
	long int rx_key_used = keyindex + buflen;
	if (rx_key_used > rx_key_ref ) {	
		long int key_file_size = get_file_size(keyfile);
		float key_presentage = (100.0*rx_key_used)/key_file_size;
		rx_key_ref = rx_key_used + 500000;
		log_info("[%d] RX key used: %ld (of %ld) %.2f %%",getpid(),rx_key_used,key_file_size,key_presentage );
		update_rx_fifo(key_presentage);
	}
	return buflen;
}

unsigned int socket_get_packet(int fd, struct sockaddr_in *sa, socklen_t *salen, char *buf, unsigned int bufsize, char* keyfile, char* inbound_counter_file)
{
	unsigned int rxbytes; 
	char *rxbuffer = malloc(bufsize);
	memset(rxbuffer, 0, bufsize);
	rxbytes = recvfrom(fd, rxbuffer, bufsize, 0, (struct sockaddr *)sa, salen);
	int decryptedbytes = decryptpacket(buf,rxbuffer,rxbytes,keyfile,inbound_counter_file);
	free (rxbuffer);
	return decryptedbytes;
}
