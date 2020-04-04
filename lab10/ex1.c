#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

extern int alphasort();
char pathname[MAXPATHLEN];

int calcularTempoCriacao(struct stat);

int main() {
	int count,i;
	struct direct **files;
	
	int file_select(const struct direct* arq) {
		if (0 == strcmp(arq->d_name, ".") || 0 == strcmp(arq->d_name, "..")) {
			return 0;
		}
		else {
			return 1;
		}
	}
	
	if (getcwd(pathname, MAXPATHLEN) == NULL) {
		printf("Error getting path\n");
		exit(0);
	}
	printf("Current Working Directory = %s\n",pathname);
	count = scandir(pathname, &files, file_select, alphasort);
	/* If no files found, make a non-selectable menu item */
	if (count <= 0) {
		printf("No files in this directory\n");
		exit(0);
	}
	printf("Number of files = %d\n", count);
	for (i = 1; i < count+1; ++i) {
		char pathArq[MAXPATHLEN];
		struct stat props;
		int dias;
		strcpy(pathArq, pathname);
		strcat(pathArq, "/");
		strcat(pathArq, files[i-1]->d_name);
		if (0 !=stat(pathArq, &props)) {
			printf("Error reading directory stats\n");
			exit(0);
		}
		dias = calcularTempoCriacao(props);
		printf("%s\tinode: %d\tsize: %d\tage: %d days\n", files[i-1]->d_name, (int) props.st_ino, (int) props.st_size, dias);
	}
	printf("\n"); /* flush buffer */
}

int calcularTempoCriacao(struct stat props) {
	long secAtual, secArq, totSecs;
	int dias;
	secAtual = (long) time(NULL);
	secArq = (long) props.st_ctime;
	totSecs = secAtual - secArq;
	dias = totSecs / 5184000; // Quantos segundos tem um dia
	return dias;
}
