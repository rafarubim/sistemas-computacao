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

int calcularTamanhoArquivos(const char* path);

int main() {
	int tamArqs;
	
	if (getcwd(pathname, MAXPATHLEN) == NULL) {
		printf("Error getting path\n");
		exit(0);
	}
	printf("Current Working Directory = %s\n", pathname);
	tamArqs = calcularTamanhoArquivos(pathname);
	printf("Directory total size in bytes = %d\n", tamArqs);
}

int calcularTamanhoArquivoRec(const char* path, DIR* pastaAberta) {
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
		int tamArq;
		// Se a pasta ainda não tiver sido aberta, abri-la e contar seu tamanho
		if (pastaAberta == NULL) {
			int tamDir;
			pastaAberta = opendir(path);
			if (pastaAberta == NULL) {
				printf("Error opening directory stream\n");
				exit(0);
			}
			
			tamDir = (int) props.st_size;
			return tamDir + calcularTamanhoArquivoRec(path, pastaAberta);
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
			// Retornar zero
			return 0;
		}
		// Se um arquivo foi lido na pasta
		
		// Se o arquivo é a própria pasta ou a pasta pai, não deve ser contado.
		// Portanto seu tamanho é considerado 0.
		if (0 == strcmp(arq->d_name, ".") || 0 == strcmp(arq->d_name, "..")) {
			tamArq = 0;
		}
		// Se o arquivo é um outro arquivo qualquer cujo tamanho deve ser contado
		// então deve-se calcular recursivamente o tamanho dele.
		else {
			// Obter path do arquivo
			strcpy(pathArq, path);
			strcat(pathArq, "/");
			strcat(pathArq, arq->d_name);
			// Calcular tamanho do arquivo recursivamente (pode ser uma pasta)
			// Novas pastas deverão ser reabertas, por isso o segundo parâmetro é NULL
			tamArq = calcularTamanhoArquivoRec(pathArq, NULL);
		}
		// Retornar tamanho do arquivo lido + tamanho dos próximos arquivos
		// da pasta. Por isso o segundo parâmetro é a pasta já aberta
		return tamArq + calcularTamanhoArquivoRec(path, pastaAberta);
	}
	// Caso base: se não for uma pasta, retornar o tamanho do arquivo
	else {
		return (int) props.st_size;
	}
}

int calcularTamanhoArquivos(const char* path) {
	// Cápsula chama recursiva passando um parâmetro NULL que indica que não há pastas abertas atualmente.
	return calcularTamanhoArquivoRec(path, NULL);
}
