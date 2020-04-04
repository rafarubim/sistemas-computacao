#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

int compararCrescente (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

void imprimirVetorInt(int vet[], int n) {
	int i;
	printf("{");
	for (i = 0; i < n; i++)
		printf("%d,", vet[i]);
	printf("}");
}

int main() {
	
	int pid, status;
	
	int vet[] = {5,6,4,3,7,2,1}, tamVet = 7;
	
	printf("Vetor antes do fork (no pai): ");
	imprimirVetorInt(vet, tamVet);
	puts("");
	
	pid = fork();
	
	if (pid != 0) {	/* Processo pai */
	
		waitpid(pid, &status, 0);
	} else {	/* Processo filho */
		
		qsort(vet, tamVet, sizeof(int), compararCrescente);	
		printf("Vetor no filho, apos ordenar: ");
		imprimirVetorInt(vet, tamVet);
		puts("");
		
		exit(0);
	}
	/* Apos filhos terminarem */
	printf("Vetor depois do fork (no pai): ");
	imprimirVetorInt(vet, tamVet);
	puts("");
	
	return 0;
}
