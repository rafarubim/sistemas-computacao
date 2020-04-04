#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NOME_FIFO "pipe"

void erro(char* msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

int main() {
	int fifo;
	char c;
	if (0 > access(NOME_FIFO, F_OK)) {
		if (mkfifo(NOME_FIFO, 0660) != 0) {
			erro("Falha ao criar pipe");
		}
	}
	if (0 > (fifo = open(NOME_FIFO, O_WRONLY))) {
		erro("Falha ao abrir pipe");
	}
	
	for(;;) {
		while(0 >= read(0, &c, sizeof(char)));
		write(fifo, &c, sizeof(char));
	}
	
	close(fifo);
	return 0;
}
