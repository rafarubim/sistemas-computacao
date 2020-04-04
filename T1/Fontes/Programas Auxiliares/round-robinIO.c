#include <stdio.h>
#include <unistd.h>

int main() {
	while(1) {
		printf("RRIO: ROUND-ROBIN de pid %d\n", getpid());
		usleep(100000);
	}
	return 0;
}
