/*********************************************************************************************
*	Módulo de definição: lstprio.h
*	Letras identificadoras: LSTP
*	Descrição:
*		Cria uma lista de prioridades de ponteiros genéricos seguindo um certo critério especificado
*		em sua criação.
*********************************************************************************************

*********************************************************************************************
*	Tipo abstrato: LSTP_tppLstPrio
*	Descrição:
*		Representa uma referência para uma lista de prioridades
*********************************************************************************************/


typedef struct _stLstPrio* LSTP_tppLstPrio;



/*********************************************************************************************
*	Função: LSTP_LstPrioCriar
*	Descrição:
*		Cria e retorna uma referência para uma lista de prioridades. Recebe 2 funções que ditarão
*		o comportamento dessa lista. Uma é a função padraoDestruirElemento, que recebe um elemento da lista 
*		(do tipo específico do usuário). Essa função é usada na destruição da lista, destruindo
*		cada elemento dela individualmente, da maneira desejada pelo usuário. A outra é a função
*		padraoPrioridades, que recebe 2 elementos da lista (do tipo específico do usuário) e os
*		compara da forma desejada. Se tal função retorna 1, então elem1 tem prioridade na lista sobre
*		elem2. Caso ela retorne 0, então elem2 possuirá prioridade na lista sobre elem1 (pode-se criar
*		assim um minheap ou um maxheap, por exemplo). Se o segundo argumento receber NULL, não haverá
*		função padrão de destruição, então destruir a lista não terá efeito nenhum em cada um de seus
*		elementos individuais. O primeiro argumento não aceita NULL.
*********************************************************************************************/
LSTP_tppLstPrio LSTP_LstPrioCriar(int (*padraoPrioridades)(void* elem1, void*elem2), void (*padraoDestruirElemento)(void* elem));

/*********************************************************************************************
*	Função: LSTP_LstPrioDestruir
*	Descrição:
*		Destroi uma lista de prioridades, liberando seu espaço alocado na memória. Se a lista
*		possuir uma função padrão de destruição de elementos, ela é chamada para cada elemento da lista.
*********************************************************************************************/
void LSTP_LstPrioDestruir(LSTP_tppLstPrio lstPrio);


/*********************************************************************************************
*	Função: LSTP_LstPrioInserir
*	Descrição:
*		Recebe uma lista de prioridades e um elemento, que é inserido nela.
*********************************************************************************************/
void LSTP_LstPrioInserir(LSTP_tppLstPrio lstPrio, void* novoElem);


/*********************************************************************************************
*	Função: LSTP_LstPrioObterPrimeiro
*	Descrição:
*		Retorna o elemento mais prioritário da lista, sem retirá-lo dela.
*********************************************************************************************/
void* LSTP_LstPrioObterPrimeiro(LSTP_tppLstPrio lstPrio);

/*********************************************************************************************
*	Função: LSTP_LstPrioPop
*	Descrição:
*		Retorna o elemento mais prioritário da lista, retirando-o.
*********************************************************************************************/
void* LSTP_LstPrioPop(LSTP_tppLstPrio lstPrio);

/*********************************************************************************************
*	Função: LSTP_LstPrioEhVazia
*	Descrição:
*		Retorna se a lista de prioridades está ou não vazia
*	Retorno:
*		0 - não está vazia
*		1 - está vazia
*********************************************************************************************/
int LSTP_LstPrioEhVazia(LSTP_tppLstPrio lstPrio);

/*********************************************************************************************
*	Função: LSTP_LstPrioBuscar
*	Descrição:
*		Serve para buscar um elemento elemBuscado em uma lista de prioridades. Se ele for encontrado,
*		ele é retirado da lista e retornado por referência no parâmetro elemRet. A função Comparar
*		é uma função definida pelo usuário que compara o elemento elemBuscado com um elemento qualquer
*		da lista. A função Comparar deve retornar 1 quando o elemento da lista for o elemento buscado e
*		qualquer outra coisa caso contrário. Se o parâmetro Comparar receber NULL, a função busca o
*		ponteiro elemBuscado na lista de prioridades.
*	Retorno:
*		0 - o elemento buscado não foi encontrado
*		1 - o elemento buscado foi encontrado, retirado da lista e retornado por referência em elemRet
*********************************************************************************************/
int LSTP_LstPrioBuscar	(LSTP_tppLstPrio lstPrio, void* elemBuscado, int (*Comparar)(void* elemBuscado, void* elemLst), void** elemRet);
