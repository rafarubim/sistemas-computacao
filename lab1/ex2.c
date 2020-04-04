#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

int main() {
	
	int pid, status;
	
	int variavel = 1;
	
	printf("A variavel possui valor %d antes do fork\n", variavel);	
	
	pid = fork();
	
	if (pid != 0) {	/* Processo pai */
	
		waitpid(pid, &status, 0);
	} else {	/* Processo filho */
		
		variavel = 5;
		printf("A variavel possui valor %d no filho\n", variavel);	
		exit(0);
	}
	/* Apos filhos terminarem */
	printf("A variavel possui valor %d apos o final do filho\n", variavel);	
	return 0;
}
