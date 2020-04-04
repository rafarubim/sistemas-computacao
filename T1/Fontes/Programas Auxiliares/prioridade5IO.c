#include <stdio.h>
#include <unistd.h>

int main() {
	while(1) {
		printf("P5IO: PRIORIDADE 5 de pid %d\n", getpid());
		usleep(100000);
	}
	return 0;
}
