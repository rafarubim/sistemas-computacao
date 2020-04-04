#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void erro(const char* msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

int main() {
	
	int arq1, arq2;
	char c;
	
	if (-1 == (arq1 = open("./Arq1.txt", O_RDONLY))) {
		erro("Arq1 inacessivel");
	}
	if (-1 == (arq2 = open("./Arq2.txt", O_WRONLY | O_CREAT | O_TRUNC, 511))) {
		erro("Arq2 inacessivel e incriavel");
	}
	
	dup2(arq1, 0);
	dup2(arq2, 1);
	
	while (1 == scanf(" %c", &c)) {
		printf("%c", c);
	}

	close(arq1);
	close(arq2);
	
	return 0;
}
