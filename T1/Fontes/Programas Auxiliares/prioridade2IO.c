#include <stdio.h>
#include <unistd.h>

int main() {
	while(1) {
		printf("P2IO: PRIORIDADE 2 de pid %d\n", getpid());
		usleep(100000);
	}
	return 0;
}
