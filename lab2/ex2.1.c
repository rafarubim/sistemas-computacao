#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <wait.h>

#define SHMKEY 8752
#define TAMMSG 144 /* tamanho máximo da mensagem */

void erro(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}

int main() {
	int shmId, pid;
	char msgDia[TAMMSG+1], *shmP;
	char* const argv[] = {"ex2.2", NULL};
	shmId = shmget(SHMKEY, (TAMMSG+1)*sizeof(char), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if (shmId == -1) {
		erro("Nao rolou criar essa memoria ai");
	}
	shmP = (char*) shmat(shmId, NULL, 0);
	if (shmP == (char*)-1) {
		erro("Impossivel fazer attach. Oh nao!");
	}
	printf("Digite sua mensagem para o mundo ouvir! (max %d caracteres)\n", TAMMSG);
	scanf(" %144[^\n]", msgDia);
	strcpy(shmP, msgDia);
	if((pid = fork()) < 0) {
		erro("Impossivel criar novo processo");
	}
	else if (pid == 0) {
		execv("./ex2.2", argv);
	}
	else {
		waitpid(pid, NULL, 0);
	}
	if (-1 == shmdt(shmP)) {
		erro("Detach nao funciona. O problema está entre o monitor e a cadeira.");
	}
	if (-1 == shmctl(shmId, IPC_RMID, NULL)) {
		erro("Memoria compartilhada indestrutivel. Chamem os incriveis!");
	}
	return 0;
}
