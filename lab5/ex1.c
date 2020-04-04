#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

void erro(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}

int main() {
	
	int pid, fd[2];
	
	if (pipe(fd) < 0) {
		erro("Pipe incriavel");
	}
	
	if ((pid = fork()) < 0) {
		erro("Processo filho incriavel");
	}
	else if(pid != 0) {
		char mensagem[20];
		close(fd[1]);
		wait(NULL);
		read(fd[0], mensagem, 20);
		printf("%s\n", mensagem);
		close(fd[0]);
	}
	else {
		close(fd[0]);
		write(fd[1], "Teste", 6);
		close(fd[1]);
		exit(0);
	}
	
	
	return 0;
}
