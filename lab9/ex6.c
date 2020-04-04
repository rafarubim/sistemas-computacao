#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <math.h>
#include <sys/time.h>

#define NUM_CARACS 4097
#define NUM_BYTES 50

int main (int argc, char *argv[]) {
	struct stat sb;
	char *p,*p_remap;
	off_t len;
	int fd;
	
	if (argc < 2) {
		fprintf (stderr, "usage: %s <file>\n", argv[0]);
		exit(1);
	}
	fd = open (argv[1], O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(1);
	}
	if (fstat(fd, &sb) == -1) {
		perror("fstat");
		exit(1);
	}
	if (!S_ISREG (sb.st_mode)) {
		fprintf(stderr, "%s is not a file\n", argv[1]);
		exit(1);
	}
	
	p = mmap(0, sb.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
	if (p == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	p_remap=mremap(p, sb.st_size , sb.st_size + NUM_BYTES, 0);
	if (p_remap == MAP_FAILED) {
		perror("remap");
		exit(1);
	}
	if (close (fd) == -1) {
		perror("close");
		exit(1);
	}
	for(len=sb.st_size-1;len<sb.st_size+NUM_CARACS;len++)
	{
		p_remap[len]='z';
	}
	if (munmap(p_remap, sb.st_size+NUM_BYTES) == -1) 
	{
		perror("remap");
		exit(1);
	}	

	if (munmap(p, sb.st_size) == -1) {
		perror("munmap");
		exit(1);
	}
	return 0;
}

