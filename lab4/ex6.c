#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#define EVER ;;

time_t relogio;

void erro(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}

void tratar(int sinal) {
	float preco;
	switch(sinal) {
		case SIGUSR1:
			printf("Inicio de chamada\n");
			relogio = time(NULL);
			break;
		case SIGUSR2:
			relogio = time(NULL) - relogio;
			if ((int) relogio <= 60) {
				preco = 2 * relogio;
			}
			else {
				preco = relogio + 60;
			}
			preco /= 100;
			printf("Fim de chamada. Tempo: %d segundos / Preco: %.2f centavos\n", (int) relogio, preco);
			break;
		default:
			erro("inconsistencia");
	}
}

int main() {
	if (-1 == (long)signal(SIGUSR1, tratar)) {
		erro("Nao instalado USR1\n");
	}
	if (-1 == (long)signal(SIGUSR2, tratar)) {
		erro("Nao instalado USR2\n");
	}
	for(EVER);
	return 0;
}
