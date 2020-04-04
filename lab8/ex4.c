#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define MAXFILA 8
#define MAXRAND 64
#define TOTPRODCONS 64
#define SLEEPPROD 1
#define SLEEPCONS 2
#define NUMPRODS 2
#define NUMCONS 2

int fila[MAXFILA];
int iniFila = 0;
int fimFila = -1;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t nao_cheio = PTHREAD_COND_INITIALIZER;
static pthread_cond_t nao_vazio = PTHREAD_COND_INITIALIZER;

int avancarIndiceCircular(int, int);
int filaEstaCheia();
int filaEstaVazia();
void* produtor(void*);
void* consumidor(void*);

int main() {
	pthread_t prod[NUMPRODS], cons[NUMCONS];
	long i;
	
	for (i = 0; i < NUMPRODS; i++) {
		pthread_create(&prod[i], NULL, produtor, (void*) i+1);
	}
	
	for (i = 0; i < NUMCONS; i++) {
		pthread_create(&cons[i], NULL, consumidor, (void*) i+1);
	}
	
	for (i = 0; i < NUMPRODS; i++) {
		pthread_join(prod[i], NULL);
	}
	for (i = 0; i < NUMCONS; i++) {
		pthread_join(cons[i], NULL);
	}
	
	return 0;
}

void* produtor(void* num) {
	int i;
	for(i = 0; i < TOTPRODCONS; i++, sleep(SLEEPPROD)) {
		int idxProxProd;
		
		/* Fechar acesso ao vetor para produzir */
		pthread_mutex_lock(&mutex);
		
		/* Testar se a fila está cheia e o produtor terá que esperar */
		while (1 == filaEstaCheia()) {
			/* Esperar até ela deixar de ser cheia */
			pthread_cond_wait(&nao_cheio, &mutex);
		}
		/* Após o wait, a fila pode ter ficado vazia ou não. Porém
		há garantia que ela não está cheia. Logo, pode-se produzir: */
		
		/* Obter posição para produzir: */
		if (1 == filaEstaVazia()) {
				/* Se está vazia */
			/* Produzir no inicio da fila */
			idxProxProd = iniFila;
			/* Atualizar tamanho da fila para após a produção */
			fimFila = iniFila;
			/* Liberar quem estava esperando ela ficar não vazia,
			agora que 1 elemento vai ser produzido */
			pthread_cond_signal(&nao_vazio);
		}
		/* Se a fila não está vazia */
		else {
				/* Se não está vazia */
			/* Obter próxima posição de produção após o final da fila */
			idxProxProd = avancarIndiceCircular(fimFila, MAXFILA);
			/* Atualizar tamanho da fila para após a produção */
			fimFila = avancarIndiceCircular(fimFila, MAXFILA);
		}
		
		/* Produzir número aleatório */
		fila[idxProxProd] = rand() % MAXRAND;
		printf("PROD %ld: Produzido: %d\n", (long) num,fila[idxProxProd]);
		
		/* Liberar acesso ao vetor */
		pthread_mutex_unlock(&mutex);
	}
	
	pthread_exit(NULL);
}

void* consumidor(void* num) {
	int i;
	for(i = 0; i < TOTPRODCONS; i++, sleep(SLEEPCONS)) {		
		/* Fechar acesso ao vetor para consumir */
		pthread_mutex_lock(&mutex);
		
		/* Se a fila esta vazia */
		while (1 == filaEstaVazia()) {
			/* Esperar ficar nao vazia */
			pthread_cond_wait(&nao_vazio, &mutex);
		}
		
		/* Se a fila esta cheia */
		if (1 == filaEstaCheia()) {
			/* Liberar para quem estava esperando ficar não cheia,
			agora que 1 elemento vai ser consumido */
			pthread_cond_signal(&nao_cheio);
		}
		
		/* Consumir primeiro item da fila */
		printf("CONS %ld: Consumido: %d\n", (long) num, fila[iniFila]);
		
		/* Se só havia um elemento */
		if (iniFila == fimFila) {
			/* Marcar fila como vazia */
			fimFila = -1;
		}
		
		/* Retirar elemento consumido da fila */
		iniFila = avancarIndiceCircular(iniFila, MAXFILA);
		
		/* Liberar acesso ao vetor */
		pthread_mutex_unlock(&mutex);
	}
	
	pthread_exit(NULL);
}

int avancarIndiceCircular(int idx, int maxVet) {
	idx = idx + 1;
	if (idx >= maxVet) {
		idx = 0;
	}
	return idx;
}

/*
	Função: filaEstaCheia
	Retorna 1 se a fila estiver cheia,
	Retorna 0 caso não possua elementos ou não esteja cheia
*/
int filaEstaCheia() {
	/* Se a fila está não vazia */
	if (0 == filaEstaVazia()) {
		int idxProx;
		/* Obter próxima posição após o final da fila */
		idxProx = avancarIndiceCircular(fimFila, MAXFILA);
		
		/* Se ver se esta posição é o início da fila,
		se sim então ela está cheia */
		if (idxProx == iniFila) {
			return 1;
		}
	}
	return 0;
}

/*
	Função: filaEstaVazia
	Retorna 1 se a fila não tiver elementos,
	Retorna 0 caso contrário
*/
int filaEstaVazia() {
	if (fimFila == -1) {
		return 1;
	}
	return 0;
}
