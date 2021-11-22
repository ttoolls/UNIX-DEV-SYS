#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 100

const char *string = "test data";

int main() {
	pid_t  pid;
	int num, fd[2];
	char line[MAX_LINE];
	if (pipe(fd) == -1) {
		perror("pipe()");
		exit(1);
	}
	if ((pid = fork()) == -1) {
		perror("fork()");
		exit(1);
	}
	if (pid > 0) { // в родителе
		close(fd[0]);
		int pipesize = fcntl(fd[1], F_GETPIPE_SZ);
		printf("PIPE Capacity %d\n",pipesize);
		write(fd[1],string,strlen(string));
		waitpid(pid, NULL, 0);
	} else { // в ребенке
		close(fd[1]);
		num = read(fd[0],line,MAX_LINE);
		write(1, line, num);
		write(1,"\n",1);
	}
	return 0;
}