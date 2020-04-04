#include <stdio.h>
#include <unistd.h>

int main() {
	while(1) {
		printf("P7IO: PRIORIDADE 7 de pid %d\n", getpid());
		usleep(100000);
	}
	return 0;
}
