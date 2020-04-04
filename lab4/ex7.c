#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void erro(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}

int main() {
	int i, pid[3];
	
	char* const argv0[] = {"./prog1", NULL};
	char* const argv1[] = {"./prog2", NULL};
	char* const argv2[] = {"./prog3", NULL};
	
	if ((pid[0] = fork()) < 0) {
		erro("Processo nao criado");
	}
	else if (pid[0] == 0) {
		execv("./prog1", argv0);
	}
	if ((pid[1] = fork()) < 0) {
		erro("Processo nao criado");
	}
	else if (pid[1] == 0) {
		execv("./prog2", argv1);
	}
	if ((pid[2] = fork()) < 0) {
		erro("Processo nao criado");
	}
	else if (pid[2] == 0) {
		execv("./prog3", argv2);
	}
	for(i = 0; i < 3; i++) {
		if (-1 == kill(pid[i], SIGSTOP)) {
			erro("Sinal de pausa nao enviado");
		}
	}
	for(i = 0;; i++) {
		int sono;
		i %= 3;
		switch(i) {
			case 0:
				sono = 1;
				break;
			case 1:
				sono = 2;
				break;
			case 2:
				sono = 2;
				break;
		}
		if (-1 == kill(pid[i], SIGCONT)) {
			erro("Sinal de pausa nao enviado");
		}
		if (-1 == kill(pid[(i+2)%3], SIGSTOP)) {
			erro("Sinal de continuacao nao enviado");
		}
		sleep(sono);
	}
	
	return 0;
}
