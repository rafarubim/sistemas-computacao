#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <stdlib.h>

#define M 4
#define N 5

void erro(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}

void imprimirMatriz(int *mat) {
	int i, j;
	for (i = 0; i < M; i++) {
		printf("|\t");
		for(j = 0; j < N; j++) {
			int idx = i*N + j;
			printf("%d\t",mat[idx]);
		}
		printf("|\n");
	}
}

int main() {
	int seg1, seg2, seg3, i, id, linha;
	int pid[M-1];
	int *mat1, *mat2, *mat3;
	seg1 = shmget(IPC_PRIVATE, M*N*sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	seg2 = shmget(IPC_PRIVATE, M*N*sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	seg3 = shmget(IPC_PRIVATE, M*N*sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if (seg1 == -1 || seg2 == -1 || seg3 == -1) {
		erro("Memoria compartilhada incriavel");
	}
	mat1 = (int*) shmat(seg1, NULL, 0);
	mat2 = (int*) shmat(seg2, NULL, 0);
	mat3 = (int*) shmat(seg3, NULL, 0);
	if (mat1 == (int*)-1 || mat2 == (int*)-1 || mat3 == (int*)-1) {
		erro("Memoria compartilhada inligavel");
	}
	for (i = 0; i < M*N; i++) { /* preencher vetores */
		mat1[i] = i;
		mat2[i] = M*N-1-i;
	}
	linha = 0; /* número da linha da matriz com a qual o processo atual deve trabalhar */
	/* o processo pai trabalhará com a linha 0 */
	
	/* cria-se M-1 processos para cuidar das outras linhas */
	for (i = 0; i < M-1; i++) {
		if ((id = fork()) < 0) { /* criar processo */
			erro("Impossivel criar processo");
		}
		else if (id == 0) {
			linha = i + 1; /* nova linha é atribuída ao processo filho */
			break; /* processo filho não deve continuar no for, pois só pai cria novos processos */
		}
		else {
			pid[i] = id; /* salvar pid do processo filho recém-criado */
		}
	}
	for (i = 0; i < N; i++) { /* cada processo calcula a soma de sua linha */
		int idx = linha*N + i;
		mat3[idx] = mat1[idx] + mat2[idx];
	}
	if (id == 0) { /* faz o detach e mata os processos filhos */
		if (-1 == shmdt(mat1) || -1 == shmdt(mat2) || -1 == shmdt(mat3)) {
			erro("Impossivel deligar memoria compartilhada do processo filho");
		}
		exit(0);
	}
	else {
		for (i = 0; i < M-1; i++) { /* pai espera cada processo terminar */
			waitpid(pid[i], NULL, 0);
		}
	}
	
	/* Imprimir matrizes finais */
	puts("Matriz 1:");
	imprimirMatriz(mat1);
	puts("Matriz 2:");
	imprimirMatriz(mat2);
	puts("Matriz soma:");
	imprimirMatriz(mat3);
	
	/* faz o detach do pai */
	if (-1 == shmdt(mat1) || -1 == shmdt(mat2) || -1 == shmdt(mat3)) {
		erro("Impossivel deligar memoria compartilhada do processo pai");
	}
	
	/* libera memoria compartilhada */
	if (-1 == shmctl(seg1, IPC_RMID, NULL) || -1 == shmctl(seg2, IPC_RMID, NULL) || -1 == shmctl(seg3, IPC_RMID, NULL)) {
		erro("Impossivel liberar memoria compartilhada");
	}
	return 0;
}
