/***************************************************************************
*
*  M�dulo de defini��o: LIS  Lista duplamente encadeada
*
*  Arquivo gerado:              LISTA.h
*  Letras identificadoras:      LIS
*
*  Descri��o do m�dulo
*     Implementa listas gen�ricas duplamente encadeadas.
*     As listas possuem uma cabe�a encapsulando o seu estado.
*
***************************************************************************/

/***** Declara��es exportadas pelo m�dulo *****/

/* Tipo refer�ncia para uma lista */

typedef struct LIS_tgLista * LIS_tppLista ;

/***********************************************************************
*
*  $FC Fun��o: LIS  &Criar lista
*
*  $ED Descri��o da fun��o
*     Cria uma lista gen�rica duplamente encadeada.
*     Os poss�veis tipos s�o desconhecidos a priori.

*     Se executou corretamente retorna o ponteiro para a lista.
*     Este ponteiro ser� utilizado pelas fun��es que manipulem esta lista.
*     Se ocorreu algum erro, por exemplo falta de mem�ria ou dados errados,
*     a fun��o retornar� NULL.
*
***********************************************************************/

void LIS_CriarLista( LIS_tppLista* pListaNovaRet, void ( * ExcluirValor ) ( void * pDado ) ) ;


/***********************************************************************
*
*  $FC Fun��o: LIS  &Destruir lista
*
*  $ED Descri��o da fun��o
*     Destr�i a lista fornecida.
*     Se ocorrer algum erro durante a destrui��o, a lista resultar�
*     estruturalmente incorreta.
*
***********************************************************************/

void LIS_DestruirLista( LIS_tppLista pLista ) ;


/***********************************************************************
*
*  $FC Fun��o: LIS  &Esvaziar lista
*
*  $ED Descri��o da fun��o
*     Elimina todos os elementos, sem contudo eliminar a lista
*
*
***********************************************************************/

void LIS_EsvaziarLista( LIS_tppLista pLista ) ;


/***********************************************************************
*
*  $FC Fun��o: LIS  &Inserir elemento antes
*
*  $ED Descri��o da fun��o
*     Insere novo elemento antes do elemento corrente.
*     Caso a lista esteja vazia, insere o primeiro elemento da lista.
*
*
***********************************************************************/

void LIS_InserirElementoAntes( LIS_tppLista pLista ,void * pValor) ;


/***********************************************************************
*
*  $FC Fun��o: LIS  &Inserir elemento ap�s
*
*  $ED Descri��o da fun��o
*     Insere novo elemento ap�s o elemento corrente.
*     Caso a lista esteja vazia, insere o primeiro elemento da lista.
*
***********************************************************************/

void LIS_InserirElementoApos( LIS_tppLista pLista , void * pValor);


/***********************************************************************
*
*  $FC Fun��o: LIS  &Excluir elemento
*
*  $ED Descri��o da fun��o
*     Exclui o elemento corrente da lista dada.
*     Se existir o elemento � esquerda do corrente ser� o novo corrente.
*     Se n�o existir e existir o elemento � direita, este se tornar� corrente.
*     Se este tamb�m n�o existir a lista tornou-se vazia.
***********************************************************************/

void LIS_ExcluirElemento( LIS_tppLista pLista ) ;


/***********************************************************************
*
*  $FC Fun��o: LIS  &Obter refer�ncia para o valor contido no elemento
*
*  $ED Descri��o da fun��o
*     Obtem a refer�ncia para o valor contido no elemento corrente da lista
***********************************************************************/

void LIS_ObterValor( LIS_tppLista pLista , void** pValorRet);


/***********************************************************************
*
*  $FC Fun��o: LIS  &Ir para o elemento inicial
*
*  $ED Descri��o da fun��o
*     Torna corrente o primeiro elemento da lista.
*     Nao faz nada se a lista est� vazia.
***********************************************************************/

void LIS_IrInicioLista( LIS_tppLista pLista );


/***********************************************************************
*
*  $FC Fun��o: LIS  &Ir para o elemento final
*
*  $ED Descri��o da fun��o
*     Torna corrente o elemento final da lista.
*     Faz nada se a lista est� vazia.
***********************************************************************/

void LIS_IrFinalLista( LIS_tppLista pLista );


/***********************************************************************
*
*  $FC Fun��o: LIS  &Avan�ar elemento
*
*  $ED Descri��o da fun��o
*     Avan�a o elemento corrente numElem elementos na lista
*     Se numElem for positivo avan�a em dire��o ao final
*     Se numElem for negativo avan�a em dire��o ao in�cio
*     Se numElem for zero somente verifica se a lista est� vazia
***********************************************************************/

void LIS_AvancarElementoCorrente(LIS_tppLista pLista ,int iNumElem);

/***********************************************************************
*
*  $FC Fun��o: LIS  &Procurar elemento contendo valor
*
*  $ED Descri��o da fun��o
*     Procura o elemento que referencia o valor dado.
*     Se o elemento for encontrado, o corrente ser� movido para o elemento encontrado.
*
***********************************************************************/

int LIS_ProcurarValor( LIS_tppLista pLista, void * pValor, int (*Comparar)(void* pElemProc, void* pElemLista)) ;


/***********************************************************************
*
*  $FC Fun��o: LIS  &Obter refer�ncia para o tamanho da lista
*
*  $ED Descri��o da fun��o
*     Obtem a refer�ncia para o tamanho da lista
*
***********************************************************************/

void LIS_ObterTamanhoLista( LIS_tppLista pLista , int* pTamanhoRet) ;


/***********************************************************************
*
*  $FC Fun��o: LIS  &Verificar se o elemento corrente esta no final da lista
*
*  $ED Descri��o da fun��o
*
*     Retorna 1 caso o elemento corrente seja o ultimo elemento da lista e 0 caso nao seja.
*
*  $EP Par�metros
*     pLista - ponteiro para a lista em questao
*
***********************************************************************/

int LIS_CorrenteEstaFinal(LIS_tppLista pLista);

/***********************************************************************
*
*  $FC Fun��o: LIS  &Verificar se o elemento corrente esta no inicio da lista
*
*  $ED Descri��o da fun��o
*     Retorna 1 caso o elemento corrente seja o primeiro elemento da lista e 0 caso nao seja.
***********************************************************************/

int LIS_CorrenteEstaInicio(LIS_tppLista pLista);

/***********************************************************************
*
*  $FC Fun��o: LIS  &Push no elemento corrente
*
*  $ED Descri��o da fun��o
*     Salva o elemento corrente para que possa posteriormente ser restaurado
*     atrav�s de um pop.
***********************************************************************/

void LIS_PushCorrente(LIS_tppLista pLista);

/***********************************************************************
*
*  $FC Fun��o: LIS  &Pop no elemento corrente
*
*  $ED Descri��o da fun��o
*     Restaura o elemento corrente que foi salvo por um push.
***********************************************************************/

void LIS_PopCorrente(LIS_tppLista pLista);

/********** Fim do m�dulo de defini��o: LIS  Lista duplamente encadeada **********/
