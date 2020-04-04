#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

int main() {
	
	int pid, status;
	
	char str1[] = "ex4Aux";
	char str2[] = "echo";
	char str3[] = "jessica eh legal";
	
	char* const argv1[] = {str1, NULL};
	char* const argv2[] = {str2, str3, NULL};
	
	pid = fork();
	
	if (pid != 0) {	/* Processo pai */
		
		execv("/usr/bin/echo", argv2);
		printf("Depois do exec do echo\n");
		waitpid(pid, &status, 0);
	} else {	/* Processo filho */
		
		execv("./ex4Aux", argv1);
		printf("Depois do exec do programa auxiliar\n");
		exit(0);
	}
	/* Apos filhos terminarem */
	
	return 0;
}
