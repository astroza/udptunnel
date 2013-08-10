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
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int strtoport(char *str, unsigned short *out)
{
	char *err;

	*out = htons((unsigned short)strtoul(str, &err, 10));

	return *err == 0;
}

void exec_script(const char *name, const char *arg)
{
	int pid, status;
	char path[255] = "";
	char *const args[] = {path, (char *)arg, NULL};
	strcat(path, "scripts/");
	strcat(path, name);

	pid = fork();
	if(pid == 0) {
		printf("  + Executing %s\n", path); 
		exit(execv(path, args));
	} else {
		waitpid(pid, &status, 0);
		if(WEXITSTATUS(status) == 0)
			puts("+ Script execution is OK");
		else
			puts("+ Script execution has a error");
	}
}

