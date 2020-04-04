#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

char pathname[MAXPATHLEN];

void imprimirArquivos(const char* path);

int main() {
	
	if (getcwd(pathname, MAXPATHLEN) == NULL) {
		printf("Error getting path\n");
		exit(0);
	}
	printf("Current Working Directory = %s\n", pathname);
	imprimirArquivos(pathname);
}

void imprimirArquivoPath(const char* path, int ident, int boolEhPasta) {
	int i;
	char* nomeArq;
	for (i = 0; i < ident; i++) {
		if (i == ident - 1) {
			if (boolEhPasta) {
				printf("*");
			}
			else {
				printf("-");
			}
		}
		printf("\t");
	}
	nomeArq = rindex(path, '/');
	nomeArq++;
	printf("%s", nomeArq);
	if (boolEhPasta) {
		printf(" (directory)");
	}
	printf("\n");
}

void imprimirArquivoRec(const char* path, DIR* pastaAberta, int ident) {
	struct stat props;
	// Ler status do arquivo passado
	if (0 != stat(path, &props)) {
		printf("Error reading directory stats\n");
		exit(0);
	}
	// Se for uma pasta
	if (S_ISDIR(props.st_mode)) {
		struct direct* arq;
		char pathArq[MAXPATHLEN];
		
		// Se a pasta ainda não tiver sido aberta, abri-la
		if (pastaAberta == NULL) {
			pastaAberta = opendir(path);
			if (pastaAberta == NULL) {
				printf("Error opening directory stream\n");
				exit(0);
			}
			imprimirArquivoPath(path, ident, 1);
			// Imprimir resto dos arquivos, aumentando a identação
			imprimirArquivoRec(path, pastaAberta, ident+1);
			return;
		}
		// Ler o próximo arquivo da pasta
		arq = readdir(pastaAberta);
		// Se não há mais arquivos na pasta
		if (arq == NULL) {
			// Fechar pasta que havia sido aberta
			if (0 != closedir(pastaAberta)) {
				printf("Error closing directory stream\n");
				exit(0);
			}			
			// Retornar
			return;
		}
		// Se um arquivo foi lido na pasta
		
		// Se o arquivo é a própria pasta ou a pasta pai, não deve ser impresso.
		// Se o arquivo é um outro arquivo qualquer que deve ser impresso
		// então deve-se imprimi-lo recursivamente.
		if (0 != strcmp(arq->d_name, ".") && 0 != strcmp(arq->d_name, "..")) {
			// Obter path do arquivo
			strcpy(pathArq, path);
			strcat(pathArq, "/");
			strcat(pathArq, arq->d_name);
			// Imprimir arquivo recursivamente (pode ser uma pasta)
			// Novas pastas deverão ser reabertas, por isso o segundo parâmetro é NULL
			imprimirArquivoRec(pathArq, NULL, ident);
		}
		// Deve-se imprimir próximos arquivos da pasta.
		// Por isso o segundo parâmetro é a pasta já aberta
		imprimirArquivoRec(path, pastaAberta, ident);
	}
	// Caso base: se não for uma pasta, imprimir o nome do arquivo com a identação adequada
	else {
		imprimirArquivoPath(path, ident, 0);
	}
}

void imprimirArquivos(const char* path) {
	// Cápsula chama recursiva passando um parâmetro NULL que indica
	// que não há pastas abertas atualmente e um parâmetro 0 que é a identação inicial.
	imprimirArquivoRec(path, NULL, 0);
}
