#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_THREADS      5
#define PULO_MAXIMO      100
#define DESCANSO_MAXIMO  1
#define DISTANCIA_PARA_CORRER 100

static int classificacao = 1;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static int cont = 0;

void *Correr(void  *sapo);

int main() {
	pthread_t threads[NUM_THREADS];
	long t;
	classificacao = 1;
	printf("Corrida iniciada ... \n");
	for(t=0;t < NUM_THREADS;t++)  {
		pthread_create(&threads[t], NULL, Correr, (void *) t);
	}
	for(t=0;t < NUM_THREADS; t++)  pthread_join(threads[t],NULL);
	printf("\n Acabou!!\n%d sapos pularam!\n", cont);
	pthread_exit(NULL);
}

void *Correr(void  *sapo) { 
	int pulos = 0;
	int distanciaJaCorrida = 0;
	while (distanciaJaCorrida <= DISTANCIA_PARA_CORRER) { 
		int pulo = rand() % PULO_MAXIMO; 
		distanciaJaCorrida += pulo; 
		pulos++; 
		printf("Sapo %ld pulou\n", (long) sapo); 
		int descanso = rand() % DESCANSO_MAXIMO; 
		sleep(descanso); 
	}
	pthread_mutex_lock(&lock);
	printf("Sapo %ld  chegou na posicao %d com %d pulos\n", (long)sapo, 
	classificacao, pulos);
	cont++;
	classificacao++;
	pthread_mutex_unlock(&lock);
	pthread_exit(NULL);
} 
