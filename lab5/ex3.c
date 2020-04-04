#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

void erro(const char* msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

int main(int argc, char** argv) {
	
	int fd[2], pid;
	
	if (argc != 3) {
		erro("Uso do programa: ./programa <Comando1> <Comando2>");
	}
	
	if (0 > pipe(fd)) {
		erro("Pipe incriavel");
	}
	
	if ((pid = fork()) < 0) {
		erro("Processo filho incriavel");
	}
	else if (pid == 0) {
		close(fd[0]);
		dup2(fd[1], 1);
		system(argv[1]);
		close(fd[1]);
		exit(0);
	}
	else {
		close(fd[1]);
		dup2(fd[0], 0);
		wait(NULL);
		system(argv[2]);
		close(fd[0]);
	}
	
	return 0;
}
