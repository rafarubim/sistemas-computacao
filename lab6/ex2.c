#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wait.h>

#define NUMREP 10
#define NUMCHAR 16

union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
};

void erro(const char* msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

// inicializa o valor do semáforo
int startSemValue(int semId);

// remove o semáforo
int delSem(int semId);

// operação P
int semaforoP(int semId);

//operação V
int semaforoV(int semId);

int main() {
	
	int pid, shmId, semId;
	char* cBuff;
	
	shmId = shmget(IPC_PRIVATE, NUMCHAR * sizeof(char), 0666 | IPC_CREAT | IPC_EXCL);
	
	if (shmId == -1) {
		erro("Falha ao criar memoria compartilhada");
	}
	
	cBuff = (char*) shmat(shmId, NULL, 0);
	
	if (cBuff == (char*) -1) {
		erro("Falha ao ligar a memoria compartilhada");
	}
	
	semId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT | IPC_EXCL);
	
	if (semId == -1) {
		erro("Falha ao criar semaforo");
	}
	if (-1 == startSemValue(semId)) {
		erro("Falha ao iniciar semaforo");
	}
	
	if ((pid = fork()) < 0) {
		erro("Falha ao criar processo");
	}
	
	if (pid == 0) { // filho
		
		int r, i;
		char c = 'a';
		
		for(r = 0; r < NUMREP; r++) {
			semaforoP(semId);
				for(i = 0; i < NUMCHAR; i++) {
					cBuff[i] = c;
					c++;
					c = (c - 'a') % 26 + 'a';
				}
			semaforoV(semId);
			sleep(2);
		}
		
		if (-1 == shmdt(cBuff)) {
			erro("Nao foi possivel desconectar de memoria compartilhada");
		}
		
		exit(0);
	}
	
	sleep(1);
	
	{
		int i;
		for(i = 0; i < NUMREP; i++) {
			semaforoP(semId);
				fwrite(cBuff, sizeof(char), NUMCHAR, stdout);
				printf("*");
				fflush(stdout);
			semaforoV(semId);
			sleep(2);
		}
	}
	
	puts("");
	
	wait(NULL);
	
	if (-1 == delSem(semId)) {
		erro("Nao foi possivel deletar semaforo");
	}
	
	if (-1 == shmdt(cBuff)) {
		erro("Nao foi possivel desconectar de memoria compartilhada");
	}
	
	if(-1 == shmctl(shmId, IPC_RMID, NULL)) {
		erro("Falha ao destruir memoria compartilhada");
	}
	
	return 0;
}

int startSemValue(int semId) {
	union semun semUnion;
	semUnion.val = 1;
	return semctl(semId, 0, SETVAL, semUnion);
}

int delSem(int semId) {
	return semctl(semId, 0, IPC_RMID);
}

int semaforoP(int semId) {
	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = -1;
	semB.sem_flg = SEM_UNDO;
	return semop(semId, &semB, 1);
}

int semaforoV(int semId) {
	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = 1;
	semB.sem_flg = SEM_UNDO;
	return semop(semId, &semB, 1);
}
