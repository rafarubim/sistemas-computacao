/***************************************************************************
*   Módulo de implementação: LIS  Lista duplamente encadeada
*
*  Arquivo gerado:              LISTA.c
*  Letras identificadoras:      LIS
*
* 
*
*
***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <stdlib.h>
#include "lista.h"

/***********************************************************************
*
*   Tipo de dados: LIS Elemento da lista
*
*
***********************************************************************/

   typedef struct tgElemLista {

         void * pValor ;
               /* Ponteiro para o valor contido no elemento */

         struct tgElemLista * pAnt ;
               /* Ponteiro para o elemento predecessor */

         struct tgElemLista * pProx ;
               /* Ponteiro para o elemento sucessor */

   } tpElemLista ;

/***********************************************************************
*
*   Tipo de dados: LIS Descritor da cabeça de lista
*
*
***********************************************************************/

   typedef struct LIS_tgLista {

         tpElemLista * pOrigemLista ;
               /* Ponteiro para a origem da lista */

         tpElemLista * pFimLista ;
               /* Ponteiro para o final da lista */

         tpElemLista * pElemCorr ;
               /* Ponteiro para o elemento corrente da lista */

         int numElem ;
               /* Número de elementos da lista */
               
         tpElemLista * pBackupCorr;
         	/* Ponteiro para um elemento da lista que ficará salvo */

         void ( * ExcluirValor ) ( void * pValor ) ;
               /* Ponteiro para a função de destruição do valor contido em um elemento */

   } LIS_tpLista ;

/***** Protótipos das funções encapuladas no módulo *****/

   void LiberarElemento( LIS_tppLista pLista ,tpElemLista  * pElem) ;

   void CriarElemento( LIS_tppLista pLista , void * pValor , tpElemLista** pElemRet) ;

   void LimparCabeca( LIS_tppLista pLista ) ;

/*****  Código das funções exportadas pelo módulo  *****/

/***************************************************************************
*
*  Função: LIS  &Criar lista
*  ****/

   void LIS_CriarLista( LIS_tppLista* pListaNovaRet, void  ( * ExcluirValor ) ( void * pDado ) )
   {

      LIS_tpLista * pLista = NULL;
	

      pLista = ( LIS_tpLista * ) malloc( sizeof( LIS_tpLista ));
      if ( pLista == NULL )
      {
		 *pListaNovaRet = NULL;
		 printf("Nao ha memoria suficiente\n");
	         exit(-1);	 
      } /* if */

       LimparCabeca( pLista );
	

      pLista->ExcluirValor = ExcluirValor;

	  *pListaNovaRet = pLista;
      

   } /* Fim função: LIS  &Criar lista */

/***************************************************************************
*
*  Função: LIS  &Destruir lista
*  ****/

    void LIS_DestruirLista( LIS_tppLista pLista )
   {

      assert( pLista != NULL );
      LIS_EsvaziarLista( pLista );

      free( pLista );

   } /* Fim função: LIS  &Destruir lista */

/***************************************************************************
*
*  Função: LIS  &Esvaziar lista
*  ****/

   void LIS_EsvaziarLista( LIS_tppLista pLista )
   {
      tpElemLista * pElem ;
      tpElemLista * pProx ;
      assert( pLista != NULL );
     
      pElem = pLista->pOrigemLista;
      while ( pElem != NULL )
      {
         pProx = pElem->pProx ;
         LiberarElemento( pLista , pElem );
	
         pElem = pProx ;
      } /* while */

      LimparCabeca( pLista ) ;
	

   } /* Fim função: LIS  &Esvaziar lista */

/***************************************************************************
*
*  Função: LIS  &Inserir elemento antes*/


   void LIS_InserirElementoAntes( LIS_tppLista pLista ,
                                           void * pValor        )
   {

      tpElemLista * pElem ;
      /* Criar elemento a inerir antes */
         CriarElemento(pLista ,pValor ,&pElem) ;
		
         if ( pElem == NULL )
         {
            printf("Nao ha memoria suficiente\n");
	    exit(-1);
         } /* if */

      /* Encadear o elemento antes do elemento corrente */

         if ( pLista->pElemCorr == NULL )
         {
            pLista->pOrigemLista = pElem ;
            pLista->pFimLista = pElem ;
         } else {
            if ( pLista->pElemCorr->pAnt != NULL )
            {
               pElem->pAnt  = pLista->pElemCorr->pAnt ;
               pLista->pElemCorr->pAnt->pProx = pElem ;
            } else
            {
               pLista->pOrigemLista = pElem ;
            } /* if */

            pElem->pProx = pLista->pElemCorr ;
            pLista->pElemCorr->pAnt = pElem ;
         } /* if */

         pLista->pElemCorr = pElem ;

   

   } /* Fim função: LIS  &Inserir elemento antes */

/***************************************************************************
*
*  Função: LIS  &Inserir elemento após
*  ****/

   void LIS_InserirElementoApos( LIS_tppLista pLista ,void * pValor)
      
   {

      tpElemLista * pElem ;
	  
         assert( pLista != NULL ) ;
      

      /* Criar elemento a inerir após */

         CriarElemento( pLista , pValor , &pElem) ;
	
         if ( pElem == NULL )
         {
            printf("Nao ha memoria suficiente\n");
	    exit(-1);
         } /* if */

      /* Encadear o elemento após o elemento */

         if ( pLista->pElemCorr == NULL )
         {
            pLista->pOrigemLista = pElem ;
            pLista->pFimLista = pElem ;
         } else
         {
            if ( pLista->pElemCorr->pProx != NULL )
            {
               pElem->pProx  = pLista->pElemCorr->pProx ;
               pLista->pElemCorr->pProx->pAnt = pElem ;
            } else{
               pLista->pFimLista = pElem ;
            } /* if */

            pElem->pAnt = pLista->pElemCorr ;
            pLista->pElemCorr->pProx = pElem ;

         } /* if */
                  
         pLista->pElemCorr = pElem ;
                  
   

   } /* Fim função: LIS  &Inserir elemento após */

/***************************************************************************
*
*  Função: LIS  &Excluir elemento
*  ****/

   void LIS_ExcluirElemento( LIS_tppLista pLista )
   {

        tpElemLista * pElem ;
	

         assert( pLista  != NULL ) ;
 
      if ( pLista->pElemCorr == NULL )
      {
	printf("Tentativa de exclusao em Lista vazia\n");
	exit(-1);
      } /* if */
      
      if (pLista->pBackupCorr == pLista->pElemCorr)
      {
	printf("Tentativa de exclusao de elemento que sofreu push\n");
	exit(-1);
      } /* if */

      pElem = pLista->pElemCorr ;

      /* Desencadeia à esquerda */

         if ( pElem->pAnt != NULL )
         {
            pElem->pAnt->pProx   = pElem->pProx ;
            pLista->pElemCorr    = pElem->pAnt ;
         } else {
            pLista->pElemCorr    = pElem->pProx ;
            pLista->pOrigemLista = pLista->pElemCorr ;
         } /* if */

      /* Desencadeia à direita */

         if ( pElem->pProx != NULL )
         {
            pElem->pProx->pAnt = pElem->pAnt ;
         } else{
            pLista->pFimLista = pElem->pAnt ;
         } /* if */

  LiberarElemento( pLista , pElem );
	 
   } /* Fim função: LIS  &Excluir elemento */

/***************************************************************************
*
*  Função: LIS  &Obter referência para o valor contido no elemento
*  ****/

void LIS_ObterValor( LIS_tppLista pLista ,void** pValorRet)
{ 
	assert( pLista != NULL );

  if ( pLista->pElemCorr == NULL )
  {
		*pValorRet = NULL;
		printf("Tentativa de obtencao de valor em Lista vazia\n");
		exit(-1);
  } /* if */

	*pValorRet = pLista->pElemCorr->pValor;
  	
} /* Fim função: LIS  &Obter referência para o valor contido no elemento */

/***************************************************************************
*
*  Função: LIS  &Ir para o elemento inicial
*  ****/

void LIS_IrInicioLista( LIS_tppLista pLista )
{
	assert(pLista != NULL);

	pLista->pElemCorr = pLista->pOrigemLista ;
} /* Fim função: LIS  &Ir para o elemento inicial */

/***************************************************************************
*
*  Função: LIS  &Ir para o elemento final
*  ****/

void LIS_IrFinalLista(LIS_tppLista pLista)
{
	assert(pLista != NULL);
	
  pLista->pElemCorr = pLista->pFimLista;
} /* Fim função: LIS  &Ir para o elemento final */

/***************************************************************************
*
*  Função: LIS  &Avançar elemento
*  ****/

void LIS_AvancarElementoCorrente( LIS_tppLista pLista ,  int iNumElem )
{
		int i ;

		tpElemLista * pElem ;

		assert( pLista != NULL ) ;
 
		/* Tratar lista vazia */

		if ( pLista->pElemCorr == NULL )
    {
			printf("Lista vazia, impossivel avancar corrente\n");
			exit(-1);
    } /* fim ativa: Tratar lista vazia */

  /* Tratar avançar para frente */

    if (iNumElem > 0)
     {
        pElem = pLista->pElemCorr ;
        for(i = iNumElem ; i > 0;i--)
        {
           if ( pElem == NULL )
           {
              break ;
           } /* if */
           pElem = pElem->pProx;
        } /* for */
        if ( pElem != NULL )
        {
           pLista->pElemCorr = pElem;
           return;
        } /* if */

        pLista->pElemCorr = pLista->pFimLista;
        printf("Foi atingido o fim da lista\n");
				exit(-1); 

     } /* fim ativa: Tratar avançar para frente */

  /* Tratar avançar para trás */

     else if ( iNumElem < 0 )
     {

        pElem = pLista->pElemCorr ;
        for( i = iNumElem ; i < 0 ; i++ )
        {
           if ( pElem == NULL )
           {
              break ;
           } /* if */
           pElem = pElem->pAnt;
        } /* for */

        if ( pElem != NULL )
        {
           pLista->pElemCorr = pElem ;
           return;
        } /* if */

        pLista->pElemCorr = pLista->pOrigemLista;
        printf("Foi atingido o fim da lista\n");
  			exit(-1);

     } /* fim ativa: Tratar avançar para trás */

  /* Tratar não avançar */


} /* Fim função: LIS  &Avançar elemento */

/***************************************************************************
*
*  Função: LIS  &Procurar elemento contendo valor
*  ****/

int LIS_ProcurarValor(LIS_tppLista pLista, void* pValor, int (*Comparar)(void* pElemProc, void* pElemLista))
{
  tpElemLista * pElem;
  
  assert(pLista != NULL) ;

  if (pLista->pElemCorr == NULL)
  {
     printf("Tentativa de busca em lista vazia\n");
     return 0;
  } /* if */
	
	LIS_IrInicioLista(pLista);
	
  for(pElem = pLista->pElemCorr; pElem != NULL; pElem = pElem->pProx)
  {
     if (1 == Comparar(pElem->pValor, pValor))
     {
        pLista->pElemCorr = pElem ;
        return 1;
     } 
  } /* for */
  
	return 0;
} /* Fim função: LIS  &Procurar elemento contendo valor */

/***************************************************************************
*
*  Função: LIS  &ObterTamanhoLista
*  ****/

void LIS_ObterTamanhoLista( LIS_tppLista pLista , int* pTamanhoRet) {
	*pTamanhoRet = pLista->numElem;
}
 /* Fim função: LIS  &ObterTamanhoLista */

/***************************************************************************
*
*  Função: LIS  &CorrenteEstaFinal
*  ****/

int LIS_CorrenteEstaFinal(LIS_tppLista pLista)
{
	if(pLista->numElem==0)
	{
		printf("Teste de corrente no final em lista vazia");
		exit(-1);
	}
	if(pLista->pElemCorr->pProx==NULL)
	{
		return 1;
	}
		return 0;
}
/* Fim função: LIS  &CorrenteEstaFinal*/

/***************************************************************************
*
*  Função: LIS  &CorrenteEstaInicio
*  ****/

int LIS_CorrenteEstaInicio(LIS_tppLista pLista)
{
	if(pLista->numElem==0)
	{
		printf("Teste de corrente no inicio em lista vazia");
		exit(-1);
	}
	if(pLista->pElemCorr->pAnt==NULL)
	{
		return 1;
	}
		return 0;
}
/* Fim função: LIS  &CorrenteEstaFinal*/

/***********************************************************************
*
*  $FC Função: LIS  &PushCorrente
*
***********************************************************************/

void LIS_PushCorrente(LIS_tppLista pLista) {
	if (pLista->pElemCorr == NULL) {
		printf("Tentativa de push em corrente em lista vazia");
		exit(-1);
	}
	pLista->pBackupCorr = pLista->pElemCorr;
}
/* Fim função: LIS  &PushCorrente*/

/***********************************************************************
*
*  $FC Função: LIS  &PopCorrente
*
***********************************************************************/
void LIS_PopCorrente(LIS_tppLista pLista) {
	if (pLista->pBackupCorr == NULL) {
		printf("Tentativa de pop em corrente, mas nenhum push foi feito");
		exit(-1);
	}
	pLista->pElemCorr = pLista->pBackupCorr;
	pLista->pBackupCorr = NULL;
}

/* Fim função: LIS  &PopCorrente*/


/*****  Código das funções encapsuladas no módulo  *****/


/***********************************************************************
*
*  $FC Função: LIS  -Liberar elemento da lista
*
*  $ED Descrição da função
*     Elimina os espaços apontados pelo valor do elemento e o
*     próprio elemento.
*
***********************************************************************/

   void LiberarElemento( LIS_tppLista  pLista , tpElemLista  * pElem)
   {

      if ( ( pLista->ExcluirValor != NULL )
        && ( pElem->pValor != NULL))
      {
         pLista->ExcluirValor( pElem->pValor ) ;
      } /* if */

      free( pElem ) ;

      pLista->numElem-- ;

   } /* Fim função: LIS  -Liberar elemento da lista */


/***********************************************************************
*
*  $FC Função: LIS  -Criar o elemento
*
***********************************************************************/

   void CriarElemento( LIS_tppLista pLista , void * pValor , tpElemLista** pElemRet)
   {

      tpElemLista * pElem ;

      pElem = ( tpElemLista * ) malloc( sizeof( tpElemLista )) ;
      if ( pElem == NULL )
      {
		 pElemRet = NULL;	
      } /* if */

      pElem->pValor = pValor ;
      pElem->pAnt   = NULL  ;
      pElem->pProx  = NULL  ;

      pLista->numElem ++ ;

	  *pElemRet = pElem	;

   } /* Fim função: LIS  -Criar o elemento */


/***********************************************************************
*
*  $FC Função: LIS  -Limpar a cabeça da lista
*
***********************************************************************/

   void LimparCabeca( LIS_tppLista pLista )
   {

      pLista->pOrigemLista = NULL ;
      pLista->pFimLista = NULL ;
      pLista->pElemCorr = NULL ;
      pLista->pBackupCorr = NULL;
      pLista->numElem   = 0 ;

   } /* Fim função: LIS  -Limpar a cabeça da lista */

/********** Fim do módulo de implementação: LIS  Lista duplamente encadeada **********/
