/* UDP Tunnel - util.c
 * 2009 - Felipe Astroza
 * Under GPLv2 license (see LICENSE)
 */

#include <stdlib.h>
#include <netinet/in.h>

int strtoport(char *str, unsigned short *out)
{
	char *err;

	*out = htons((unsigned short)strtoul(str, &err, 10));

	return *err == 0;
}
