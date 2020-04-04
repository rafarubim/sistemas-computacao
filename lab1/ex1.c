#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

int main() {
	
	int pid, status;
	
	pid = fork();
	
	if (pid != 0) {	/* Processo pai */
		int meuPid = getpid();
		printf("Ola! O pid do pai eh: %d\n", meuPid);
		waitpid(pid, &status, 0);
	} else {	/* Processo filho */
		pid = getpid();
		printf("Oiii! O pid do filho eh: %d\n", pid);
		exit(0);
	}
	
	return 0;
}
