#include <stdio.h>
#include <unistd.h>
#include <signal.h>


unsigned long cont=0;

void avisar(int sig) {
	printf("RRCPU: Resumindo ROUND-ROBIN de pid %d. Contador: %lu\n", getpid(), cont);
}

int main() {
	
	int i,j=0;
	int vet[50];
	if (SIG_ERR == signal(SIGCONT, avisar)) {
		printf("RRCPU: Erro ao configurar tratador de SIGCONT\n");
		return 1;
	}
	for(i=0;i<50;i++){
		vet[i]=i+1;
	}
	printf("RRCPU: Iniciando ROUND-ROBIN de pid %d. Contador: %lu\n", getpid(), cont);
	while(1) {
		for(i=0;i<50;i++){
			if(vet[i]!=j)
				j++;
			}
		j=0;
		cont++;
		usleep(100000);
	}
	return 0;
}
