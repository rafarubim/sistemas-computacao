#include <stdio.h>
#include <unistd.h>

int main() {
	while(1) {
		printf("P6IO: PRIORIDADE 6 de pid %d\n", getpid());
		usleep(100000);
	}
	return 0;
}
