#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define EVER ;;
#define TROCAS 10
#define DELAY 1

void erro(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}

void imprimeFilho1(int sinal) {
	printf("OI EU SOU O FILHO 1\n");
}

void imprimeFilho2(int sinal) {
	printf("OI EU SOU O FILHO 2\n");
}

int main() {
	
	pid_t pid1, pid2;
	int i;
	if ((pid1 = fork()) < 0) {
		erro("Deu merds 1");
	}
	else if (pid1 == 0) {
		signal(SIGCONT, imprimeFilho1);
		for(EVER);
	}
	else {
		if ((pid2 = fork()) < 0) {
			erro("Deu merds 2");
		}
		else if (pid2 == 0) {
			signal(SIGCONT, imprimeFilho2);
			for(EVER);
		}
		else {
			sleep(1);
			for (i = 0; i < TROCAS+1; i++) {
				pid_t start, stop;
				if (i%2 == 0) {
					start = pid1;
					stop = pid2;
				}
				else {
					start = pid2;
					stop = pid1;
				}
				if (-1 == kill(stop, SIGSTOP)) {
					erro("Processo imparavel");
				}
				if (-1 == kill(start, SIGCONT)) {
					erro("Processo incomecavel");
				}
				sleep(DELAY);
			}
		}
	}
	if (-1 == kill(pid1, SIGKILL)) {
		erro("Processo imortal");
	}
	if (-1 == kill(pid2, SIGKILL)) {
		erro("Processo imortal");
	}
	return 0;
}
