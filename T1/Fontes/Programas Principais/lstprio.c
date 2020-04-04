/*********************************************************************************************
*	Módulo de implementação: lstprio.c
*	Letras identificadoras: LSTP
*	Descrição:
*		Cria uma lista de prioridades de ponteiros genéricos seguindo um certo critério especificado
*		em sua criação.
*********************************************************************************************/

#include "lstprio.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_INICIAL 20
	/* Tamanho máximo temporário do heap (aumenta de 10 em 10 caso ultrapassado o valor máximo) */

/*********************************************************************************************
*	Tipo estruturado: _stLstPrio
*	Descrição:
*		Representa uma lista de prioridades.
*********************************************************************************************/
struct _stLstPrio {
  int max; /* tamanho maximo do heap */
  int pos; /* proxima posicao disponivel no vetor */ 
  void** prioridade; /* vetor das prioridades */
  int (*padraoPrioridades)(void* elem1, void*elem2); /* função padrão que define prioridades da lista */
  void (*padraoDestruirElemento)(void* elem); /* função padrão de destruição dos elementos da lista */
};

/*********************************************************************************************
*			Definições de Funções Estáticas
*/

static void corrige_abaixo(LSTP_tppLstPrio heap, int inicial);

static void corrige_acima(LSTP_tppLstPrio heap, int pos);

static void erro(const char* msg);

static void troca(int a, int b, void** v);

static int compararPadrao(void* elemBuscado, void* elemLista);

/*********************************************************************************************
*			Implementações de Funções Exportadas
*/

LSTP_tppLstPrio LSTP_LstPrioCriar(int (*padraoPrioridades)(void* elem1, void*elem2), void (*padraoDestruirElemento)(void* elem)) {
	LSTP_tppLstPrio heap = (LSTP_tppLstPrio) malloc(sizeof(struct _stLstPrio));
	heap->max = MAX_INICIAL;
  heap->pos = 0;
  heap->prioridade= (void**) malloc(heap->max*sizeof(void*));
  assert(heap->prioridade);
  if (padraoPrioridades == NULL) {
  	erro("Voce precisa passar uma funcao de padrao de prioridades ao criar um heap.");
  }
  heap->padraoPrioridades = padraoPrioridades;
  heap->padraoDestruirElemento = padraoDestruirElemento;
  return heap;
}

void LSTP_LstPrioDestruir(LSTP_tppLstPrio lstPrio) {
	int i;
	if (lstPrio->padraoDestruirElemento != NULL) {
		for (i = 0; i < lstPrio->pos; i++) {
			lstPrio->padraoDestruirElemento(lstPrio->prioridade[i]);
		}
	}
	free(lstPrio->prioridade);
	free(lstPrio);
}

void LSTP_LstPrioInserir(LSTP_tppLstPrio lstPrio, void* novoElem) {
	if (lstPrio->pos < lstPrio->max) {
		lstPrio->prioridade[lstPrio->pos] = novoElem; 
		corrige_acima(lstPrio, lstPrio->pos); 
		lstPrio->pos++;
	} 
	else { /* Se não há espaço, aumentar tamanho máximo do heap */
		void** novoVec;
		int i;
		lstPrio->max += 10;
		novoVec = (void**) malloc(lstPrio->max*sizeof(void*));
		assert(novoVec);
		for (i = 0; i < lstPrio->max - 10; i++) {
			novoVec[i] = lstPrio->prioridade[i];
		}
		free(lstPrio->prioridade);
		lstPrio->prioridade = novoVec;
		LSTP_LstPrioInserir(lstPrio, novoElem);
	}
}

void* LSTP_LstPrioObterPrimeiro(LSTP_tppLstPrio lstPrio) {
	if (lstPrio->pos <= 0) {
		erro("Acesso em lista de prioridades VAZIA!");
	}
	return lstPrio->prioridade[0];
}

void* LSTP_LstPrioPop(LSTP_tppLstPrio lstPrio) {
	if (lstPrio->pos > 0) {
		void* topo = lstPrio->prioridade[0];
		lstPrio->prioridade[0] = lstPrio->prioridade[lstPrio->pos-1];
		lstPrio->pos--;
		corrige_abaixo(lstPrio, 0);
		return topo;
	}
	else {
		 erro("Pop em lista de prioridades VAZIA!");
		 return NULL;
	}
}

int LSTP_LstPrioEhVazia(LSTP_tppLstPrio lstPrio) {
	if (lstPrio->pos <= 0) {
		return 1;
	}
	return 0;
}

int LSTP_LstPrioBuscar	(LSTP_tppLstPrio lstPrio, void* elemBuscado,int (*Comparar)(void* elemBuscado, void* elemLst), void** elemRet) {
	int i = 0;
	/* Se o usuário não especificou função de comparação, usar função padrão */
	if (Comparar == NULL) {
		Comparar = compararPadrao;
	}
	for (i = 0; i < lstPrio->pos; i++) {
		if (1 == Comparar(elemBuscado, lstPrio->prioridade[i])) {
			*elemRet = lstPrio->prioridade[i];
			lstPrio->prioridade[i] = lstPrio->prioridade[lstPrio->pos-1];
			lstPrio->pos--;
			if (i == 0) {
				corrige_abaixo(lstPrio, 0);
			}
			else if (i != lstPrio->pos-1) {
				int pai = (i-1) / 2;
				if (1 == lstPrio->padraoPrioridades(lstPrio->prioridade[pai], lstPrio->prioridade[i])) {
					corrige_abaixo(lstPrio, i);
				}
				else {
					corrige_acima(lstPrio, i);
				}
			}
			return 1;
		}
	}
	*elemRet = NULL;
	return 0;
}

/*********************************************************************************************
*			Implementações de Funções Estáticas
*/

/*********************************************************************************************
*	Função: corrige_abaixo
*	Descrição:
*		Pega o nó pai do heap e corrige-o passo a passo atá o fim dele
*********************************************************************************************/
static void corrige_abaixo(LSTP_tppLstPrio heap, int inicial){
  int pai = inicial;
	while(2 * pai + 1 < heap->pos) {
    int filho_esq = 2 * pai + 1;
    int filho_dir = 2 * pai + 2;
    int filho;
    if (filho_dir >= heap->pos) {
    	filho_dir = filho_esq;
    }
		if (heap->padraoPrioridades(heap->prioridade[filho_esq], heap->prioridade[filho_dir])  == 1)
			filho=filho_esq;
		else
			filho=filho_dir;
    if (heap->padraoPrioridades(heap->prioridade[pai],heap->prioridade[filho]) != 1)
			troca(pai, filho, heap->prioridade);
    else
			break;
    pai = filho;
  }
}

/*********************************************************************************************
*	Função: corrige_acima
*	Descrição:
*		Pega o nó do heap na posição pos e corrige-o passo a passo até o topo dele
*********************************************************************************************/
static void corrige_acima(LSTP_tppLstPrio heap, int pos) {
  int pai;
  while (pos > 0) {
    pai = (pos - 1) / 2;
    if (heap->padraoPrioridades(heap->prioridade[pai], heap->prioridade[pos]) != 1)
      troca(pos, pai, heap->prioridade);
    else 
      break;
    pos = pai;
  }
}

/*********************************************************************************************
*	Função: erro
*	Descrição:
*		Apresenta uma mensagem de erro e fecha o programa, retornando 1
*********************************************************************************************/
static void erro(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}

/*********************************************************************************************
*	Função: troca
*	Descrição:
*		Troca dois elementos de um vetor genérico nos índices a e b
*********************************************************************************************/
static void troca(int a, int b, void** v) {
  void* f = v[a];
  v[a] = v[b];
  v[b] = f;
}

/*********************************************************************************************
*	Função: comparar (padrao)
*	Descrição:
*		Função Comparar padrão utilizada na função LSTP_LstPrioBuscar quando o usuário usa NULL
*	Retorno:
*		1	-	caso os parametros sejam iguais
*		0	-	caso os parâmetros sejam diferentes
*********************************************************************************************/
static int compararPadrao(void* elemBuscado, void* elemLista) {
	if (elemBuscado == elemLista) {
		return 1;
	}
	else {
		return 0;
	}
}
