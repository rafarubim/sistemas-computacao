#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <math.h>
#include <sys/time.h>

int main (int argc, char *argv[]) {
	struct stat sb;
	off_t len;
	char *p, c, *bufferArq;
	int fd, i;
	long pgsize, numpgs, elapsedTime;
	struct timeval t0, t1;
	
	if (argc < 2) {
		fprintf (stderr, "usage: %s <file>\n", argv[0]);
		exit(1);
	}
	fd = open (argv[1], O_RDONLY);
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
	pgsize = sysconf(_SC_PAGESIZE);
	numpgs = (long) ceil((double) sb.st_size / pgsize);
	printf("\nNumero de pags utilizadas pelo arquivo: %ld\n\n", numpgs);
	
	printf("Arquivo impresso por leitura do HD:\n\n");
	
	gettimeofday(&t0, NULL);
	while(0 < read(fd, &c, sizeof(char))) {
		putchar(c);
	}
	gettimeofday(&t1, NULL);
	
	elapsedTime = (t1.tv_sec - t0.tv_sec) * (long) 1e6 + (t1.tv_usec - t0.tv_usec);
	printf("\nTempo de impressao de arquivo do HD: %ld microssegundos\n\n", elapsedTime);
	
	bufferArq = (char*) malloc(sizeof(char) * sb.st_size);
	
	printf("Arquivo impresso por leitura do HD de uma soh vez:\n\n");
	
	lseek(fd, 0, SEEK_SET);
	gettimeofday(&t0, NULL);
	read(fd, bufferArq, sizeof(char) * sb.st_size);
	for (i = 0; i < sb.st_size; i++) {
		putchar(bufferArq[i]);
	}
	gettimeofday(&t1, NULL);
	
	elapsedTime = (t1.tv_sec - t0.tv_sec) * (long) 1e6 + (t1.tv_usec - t0.tv_usec);
	printf("\nTempo de impressao de arquivo do HD de uma soh vez: %ld microssegundos\n\n", elapsedTime);
	
	gettimeofday(&t0, NULL);
	p = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
	gettimeofday(&t1, NULL);
	
	elapsedTime = (t1.tv_sec - t0.tv_sec) * (long) 1e6 + (t1.tv_usec - t0.tv_usec);
	printf("Tempo de mapeamento do arquivo para RAM: %ld microssegundos\n\n", elapsedTime);
	
	if (p == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	if (close (fd) == -1) {
		perror("close");
		exit(1);
	}
	
	printf("Arquivo impresso por leitura do mapeamento da RAM:\n\n");
	
	gettimeofday(&t0, NULL);
	for (len = 0; len < sb.st_size; len++) {
		putchar(p[len]);
	}
	gettimeofday(&t1, NULL);
	
	elapsedTime = (t1.tv_sec - t0.tv_sec) * (long) 1e6 + (t1.tv_usec - t0.tv_usec);
	printf("\nTempo de impressao de arquivo mapeado: %ld microssegundos\n", elapsedTime);
	
	if (munmap(p, sb.st_size) == -1) {
		perror("munmap");
		exit(1);
	}
	return 0;
}
