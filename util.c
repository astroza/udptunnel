#include <stdlib.h>
#include <netinet/in.h>

int strtoport(char *str, unsigned short *out)
{
	char *err;

	*out = htons((unsigned short)strtoul(str, &err, 10));

	return *err == 0;
}
