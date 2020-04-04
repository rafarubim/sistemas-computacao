#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_ENTRADA "EntradaServidor"
#define FIFO_SAIDA "SaidaServidor"

void erro(const char* msg) {
	fprintf(stderr, "%s\n", msg);
	exit(-1);
}

int main() {
	int fdFifoEntrada, fdFifoSaida;
	if (0 != access(FIFO_ENTRADA, F_OK)) {
		if (0 != mkfifo(FIFO_ENTRADA, 0660)) {
			erro("Falha ao criar fifo de entrada do servidor");
		}
	}
	if (0 != access(FIFO_SAIDA, F_OK)) {
		if (0 != mkfifo(FIFO_SAIDA, 0660)) {
			erro("Falha ao criar fifo de saida do servidor");
		}
	}
	if (0 > (fdFifoEntrada = open(FIFO_ENTRADA, O_WRONLY))) {
		erro("Falha ao acessar fifo de entrada do servidor");
	}
	if (0 > (fdFifoSaida = open(FIFO_SAIDA, O_RDONLY))) {
		erro("Falha ao acessar fifo de saida do servidor");
	}
	for(;;) {
		char c;
		while(1) {
			if (0 < read(0, &c, sizeof(char)) && c >= 'a' && c <= 'z') {
				write(fdFifoEntrada, &c, sizeof(char));
				break;
			}
		}
		if (0 < read(fdFifoSaida, &c, sizeof(char))) {
			write(1, &c, sizeof(char));
		}
	}
	
	close(fdFifoEntrada);
	close(fdFifoSaida);
	return 0;
}
