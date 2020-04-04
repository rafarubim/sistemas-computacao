#include <stdio.h>
#include <unistd.h>

int main() {
	while(1) {
		printf("RTIO: REAL-TIME de pid %d\n", getpid());
		usleep(100000);
	}
	return 0;
}
