/***************************************************************************
*
*  Módulo de definição: LIS  Lista duplamente encadeada
*
*  Arquivo gerado:              LISTA.h
*  Letras identificadoras:      LIS
*
*  Descrição do módulo
*     Implementa listas genéricas duplamente encadeadas.
*     As listas possuem uma cabeça encapsulando o seu estado.
*
***************************************************************************/

/***** Declarações exportadas pelo módulo *****/

/* Tipo referência para uma lista */

typedef struct LIS_tgLista * LIS_tppLista ;

/***********************************************************************
*
*  $FC Função: LIS  &Criar lista
*
*  $ED Descrição da função
*     Cria uma lista genérica duplamente encadeada.
*     Os possíveis tipos são desconhecidos a priori.

*     Se executou corretamente retorna o ponteiro para a lista.
*     Este ponteiro será utilizado pelas funções que manipulem esta lista.
*     Se ocorreu algum erro, por exemplo falta de memória ou dados errados,
*     a função retornará NULL.
*
***********************************************************************/

void LIS_CriarLista( LIS_tppLista* pListaNovaRet, void ( * ExcluirValor ) ( void * pDado ) ) ;


/***********************************************************************
*
*  $FC Função: LIS  &Destruir lista
*
*  $ED Descrição da função
*     Destrói a lista fornecida.
*     Se ocorrer algum erro durante a destruição, a lista resultará
*     estruturalmente incorreta.
*
***********************************************************************/

void LIS_DestruirLista( LIS_tppLista pLista ) ;


/***********************************************************************
*
*  $FC Função: LIS  &Esvaziar lista
*
*  $ED Descrição da função
*     Elimina todos os elementos, sem contudo eliminar a lista
*
*
***********************************************************************/

void LIS_EsvaziarLista( LIS_tppLista pLista ) ;


/***********************************************************************
*
*  $FC Função: LIS  &Inserir elemento antes
*
*  $ED Descrição da função
*     Insere novo elemento antes do elemento corrente.
*     Caso a lista esteja vazia, insere o primeiro elemento da lista.
*
*
***********************************************************************/

void LIS_InserirElementoAntes( LIS_tppLista pLista ,void * pValor) ;


/***********************************************************************
*
*  $FC Função: LIS  &Inserir elemento após
*
*  $ED Descrição da função
*     Insere novo elemento apás o elemento corrente.
*     Caso a lista esteja vazia, insere o primeiro elemento da lista.
*
***********************************************************************/

void LIS_InserirElementoApos( LIS_tppLista pLista , void * pValor);


/***********************************************************************
*
*  $FC Função: LIS  &Excluir elemento
*
*  $ED Descrição da função
*     Exclui o elemento corrente da lista dada.
*     Se existir o elemento à esquerda do corrente será o novo corrente.
*     Se não existir e existir o elemento à direita, este se tornará corrente.
*     Se este também não existir a lista tornou-se vazia.
***********************************************************************/

void LIS_ExcluirElemento( LIS_tppLista pLista ) ;


/***********************************************************************
*
*  $FC Função: LIS  &Obter referência para o valor contido no elemento
*
*  $ED Descrição da função
*     Obtem a referência para o valor contido no elemento corrente da lista
***********************************************************************/

void LIS_ObterValor( LIS_tppLista pLista , void** pValorRet);


/***********************************************************************
*
*  $FC Função: LIS  &Ir para o elemento inicial
*
*  $ED Descrição da função
*     Torna corrente o primeiro elemento da lista.
*     Nao faz nada se a lista está vazia.
***********************************************************************/

void LIS_IrInicioLista( LIS_tppLista pLista );


/***********************************************************************
*
*  $FC Função: LIS  &Ir para o elemento final
*
*  $ED Descrição da função
*     Torna corrente o elemento final da lista.
*     Faz nada se a lista está vazia.
***********************************************************************/

void LIS_IrFinalLista( LIS_tppLista pLista );


/***********************************************************************
*
*  $FC Função: LIS  &Avançar elemento
*
*  $ED Descrição da função
*     Avança o elemento corrente numElem elementos na lista
*     Se numElem for positivo avança em direção ao final
*     Se numElem for negativo avança em direção ao início
*     Se numElem for zero somente verifica se a lista está vazia
***********************************************************************/

void LIS_AvancarElementoCorrente(LIS_tppLista pLista ,int iNumElem);

/***********************************************************************
*
*  $FC Função: LIS  &Procurar elemento contendo valor
*
*  $ED Descrição da função
*     Procura o elemento que referencia o valor dado.
*     Se o elemento for encontrado, o corrente será movido para o elemento encontrado.
*
***********************************************************************/

int LIS_ProcurarValor( LIS_tppLista pLista, void * pValor, int (*Comparar)(void* pElemProc, void* pElemLista)) ;


/***********************************************************************
*
*  $FC Função: LIS  &Obter referência para o tamanho da lista
*
*  $ED Descrição da função
*     Obtem a referência para o tamanho da lista
*
***********************************************************************/

void LIS_ObterTamanhoLista( LIS_tppLista pLista , int* pTamanhoRet) ;


/***********************************************************************
*
*  $FC Função: LIS  &Verificar se o elemento corrente esta no final da lista
*
*  $ED Descrição da função
*
*     Retorna 1 caso o elemento corrente seja o ultimo elemento da lista e 0 caso nao seja.
*
*  $EP Parâmetros
*     pLista - ponteiro para a lista em questao
*
***********************************************************************/

int LIS_CorrenteEstaFinal(LIS_tppLista pLista);

/***********************************************************************
*
*  $FC Função: LIS  &Verificar se o elemento corrente esta no inicio da lista
*
*  $ED Descrição da função
*     Retorna 1 caso o elemento corrente seja o primeiro elemento da lista e 0 caso nao seja.
***********************************************************************/

int LIS_CorrenteEstaInicio(LIS_tppLista pLista);

/***********************************************************************
*
*  $FC Função: LIS  &Push no elemento corrente
*
*  $ED Descrição da função
*     Salva o elemento corrente para que possa posteriormente ser restaurado
*     através de um pop.
***********************************************************************/

void LIS_PushCorrente(LIS_tppLista pLista);

/***********************************************************************
*
*  $FC Função: LIS  &Pop no elemento corrente
*
*  $ED Descrição da função
*     Restaura o elemento corrente que foi salvo por um push.
***********************************************************************/

void LIS_PopCorrente(LIS_tppLista pLista);

/********** Fim do módulo de definição: LIS  Lista duplamente encadeada **********/
