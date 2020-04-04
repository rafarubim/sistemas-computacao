#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wait.h>

#define NUMREP 100000

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
	int* shmBuff;
	
	shmId = shmget(IPC_PRIVATE, sizeof(int), 0666 | IPC_CREAT | IPC_EXCL);
	
	if (shmId == -1) {
		erro("Falha ao criar memoria compartilhada");
	}
	
	shmBuff = (int*) shmat(shmId, NULL, 0);
	
	if (shmBuff == (int*) -1) {
		erro("Falha ao ligar a memoria compartilhada");
	}
	
	semId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT | IPC_EXCL);
	
	if (semId == -1) {
		erro("Falha ao criar semaforo");
	}
	if (-1 == startSemValue(semId)) {
		erro("Falha ao iniciar semaforo");
	}
	
	*shmBuff = 0;
	
	if ((pid = fork()) < 0) {
		erro("Falha ao criar processo");
	}
	
	if (pid == 0) { // filho
		int i;
		for(i = 0; i < NUMREP; i++) {
			semaforoP(semId);
				(*shmBuff)++;
			semaforoV(semId);
		}
		
		if (-1 == shmdt(shmBuff)) {
			erro("Nao foi possivel desconectar de memoria compartilhada");
		}
		
		exit(0);
	}
	
	{
		int i;
		for(i = 0; i < NUMREP; i++) {
			semaforoP(semId);
				*shmBuff += 5;
			semaforoV(semId);
		}
	}
	
	wait(NULL);
	
	printf("A soma final foi: %d (esperado: %d)\n", *shmBuff, NUMREP*6);
	
	if (-1 == delSem(semId)) {
		erro("Nao foi possivel deletar semaforo");
	}
	
	if (-1 == shmdt(shmBuff)) {
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
