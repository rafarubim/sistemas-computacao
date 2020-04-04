#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265
#define NELEMS 15
#define NTHREADS 3

void erro(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}


int main() {
	int shmIdVec, shmIdRes, i, id, idxInicial, idxFinal, numProcurados, idxRes, numProc, posProc;
	int pid[NTHREADS-1];
	int *shmVec, *shmRes;
	shmIdVec = shmget(IPC_PRIVATE, NELEMS*sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmIdRes = shmget(IPC_PRIVATE, NTHREADS*sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if (shmIdVec == -1 || shmIdRes == -1) {
		erro("Memoria compartilhada incriavel");
	}
	shmVec = (int*) shmat(shmIdVec, NULL, 0);
	shmRes = (int*) shmat(shmIdRes, NULL, 0);
	if (shmVec == (int*)-1 || shmRes == (int*)-1) {
		erro("Memoria compartilhada inligavel");
	}
	
	for (i = 0; i < NELEMS; i++) { /* preencher vetor */
		shmVec[i] = (int) 100*sin(i*2.*PI/NELEMS);
	}
	
	printf("Vetor: {"); /* imprimir vetor */
	for (i = 0; i < NELEMS; i++) {
		printf("%d,", shmVec[i]);
	}
	printf("}\n");
	
	printf("RESPONDA RAPIDO! Qual elemento voce deseja procurar?\n");
	scanf(" %d", &numProc);
	
	numProcurados = NELEMS/NTHREADS; /* número de elementos que cada thread escaneia do vetor */
	idxInicial = (NTHREADS - 1) * numProcurados; /* processo pai escaneia últimos elementos */
	idxFinal = NELEMS - 1;
	idxRes = NTHREADS - 1; /* Posição utilizada pelo processo pai para salvar o resultado de sua busca no vetor de resultados */
	
	/* cria-se NTHREADS-1 processos para procurar no vetor */
	for (i = 0; i < NTHREADS-1; i++) {
		if ((id = fork()) < 0) { /* criar processo */
			erro("Impossivel criar processo");
		}
		else if (id == 0) {
			idxInicial = i * numProcurados;
			idxFinal = (i + 1) * numProcurados - 1;
			idxRes = i;
			break; /* processo filho não deve continuar no for, pois só pai cria novos processos */
		}
		else {
			pid[i] = id; /* salvar pid do processo filho recém-criado */
		}
	}
	shmRes[idxRes] = -1; /* Vetor de resultados começa com -1 (que significa que numero procurado nao foi encontrado) */
	for (i = idxInicial; i <= idxFinal; i++) { /* cada processo escaneia sua parte do vetor */
		if (shmVec[i]  == numProc) {
			shmRes[idxRes] = i; /* se numero procurado foi encontrado, o processo salva na area de resultados relativa à ele */
			break;
		}
	}
	if (id == 0) { /* faz o detach e mata os processos filhos */
		if (-1 == shmdt(shmVec) || -1 == shmdt(shmRes)) {
			erro("Impossivel deligar memoria compartilhada do processo filho");
		}
		exit(0);
	}
	else {
		for (i = 0; i < NTHREADS-1; i++) { /* pai espera cada processo filho terminar */
			waitpid(pid[i], NULL, 0);
		}
	}
	
	posProc = -1;
	for (i = 0; i < NTHREADS; i++) { /* Pai procura se o numero foi encontrado no vetor de resultados */
		if (shmRes[i] != -1) {
			posProc = shmRes[i];
			break;
		}
	}
	
	switch(posProc) {
		case -1:
			printf("O numero procurado nao esta no vetor. Que pena!\n");
			break;
		default:
			printf("O numero procurado esta na posicao %d do vetor. Que incrivel!\n", posProc+1);
	}
	
	/* faz o detach do pai */
	if (-1 == shmdt(shmVec) || -1 == shmdt(shmRes)) {
		erro("Impossivel deligar memoria compartilhada do processo pai");
	}
	
	/* libera memoria compartilhada */
	if (-1 == shmctl(shmIdVec, IPC_RMID, NULL) || -1 == shmctl(shmIdRes, IPC_RMID, NULL)) {
		erro("Impossivel liberar memoria compartilhada");
	}
	return 0;
}
