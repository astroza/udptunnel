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

