#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define _GNU_SOURCE

void *memorypool;

void *myallocate(int n) {
	return sbrk(n);
}

void initmemorypool(void) {
	memorypool = sbrk(0);
}

void resetmemorypool(void) {
	brk(memorypool);
}

int main() {
	void* p1,* p2,* p3,* p4;
	initmemorypool();
	p1 = myallocate(sizeof(int));
	p2 = myallocate(sizeof(int));
	p3 = myallocate(sizeof(int));
	resetmemorypool();
	p4 = myallocate(0);
	printf("%ld\n", (long) p1);
	printf("%ld\n", (long) p2);
	printf("%ld\n", (long) p3);
	printf("%ld\n", (long) p4);
	return 0;
}
