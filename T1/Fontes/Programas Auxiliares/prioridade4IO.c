#include <stdio.h>
#include <unistd.h>

int main() {
	while(1) {
		printf("P4IO: PRIORIDADE 4 de pid %d\n", getpid());
		usleep(100000);
	}
	return 0;
}
