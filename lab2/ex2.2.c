#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdlib.h>

#define SHMKEY 8752
#define TAMMSG 144 /* tamanho máximo da mensagem */

void erro(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}

int main() {
	int shmId;
	char *shmP;
	shmId = shmget(SHMKEY, TAMMSG, S_IRUSR | S_IWUSR);
	if (shmId == -1) {
		erro("Memoria compartilhada inacessivel");
	}
	shmP = (char*) shmat(shmId, NULL, 0);
	if (shmP == (char*)-1) {
		erro("Nao houve attach :(");
	}
	printf("A mensagem do dia eh: %s\n", shmP);
	if (-1 == shmdt(shmP)) {
		erro("Detach nao funciona. O problema está entre o monitor e a cadeira.");
	}
	return 0;
}
