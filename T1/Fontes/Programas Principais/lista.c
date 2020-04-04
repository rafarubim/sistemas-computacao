/***************************************************************************
*   M�dulo de implementa��o: LIS  Lista duplamente encadeada
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
*   Tipo de dados: LIS Descritor da cabe�a de lista
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
               /* N�mero de elementos da lista */
               
         tpElemLista * pBackupCorr;
         	/* Ponteiro para um elemento da lista que ficar� salvo */

         void ( * ExcluirValor ) ( void * pValor ) ;
               /* Ponteiro para a fun��o de destrui��o do valor contido em um elemento */

   } LIS_tpLista ;

/***** Prot�tipos das fun��es encapuladas no m�dulo *****/

   void LiberarElemento( LIS_tppLista pLista ,tpElemLista  * pElem) ;

   void CriarElemento( LIS_tppLista pLista , void * pValor , tpElemLista** pElemRet) ;

   void LimparCabeca( LIS_tppLista pLista ) ;

/*****  C�digo das fun��es exportadas pelo m�dulo  *****/

/***************************************************************************
*
*  Fun��o: LIS  &Criar lista
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
      

   } /* Fim fun��o: LIS  &Criar lista */

/***************************************************************************
*
*  Fun��o: LIS  &Destruir lista
*  ****/

    void LIS_DestruirLista( LIS_tppLista pLista )
   {

      assert( pLista != NULL );
      LIS_EsvaziarLista( pLista );

      free( pLista );

   } /* Fim fun��o: LIS  &Destruir lista */

/***************************************************************************
*
*  Fun��o: LIS  &Esvaziar lista
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
	

   } /* Fim fun��o: LIS  &Esvaziar lista */

/***************************************************************************
*
*  Fun��o: LIS  &Inserir elemento antes*/


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

   

   } /* Fim fun��o: LIS  &Inserir elemento antes */

/***************************************************************************
*
*  Fun��o: LIS  &Inserir elemento ap�s
*  ****/

   void LIS_InserirElementoApos( LIS_tppLista pLista ,void * pValor)
      
   {

      tpElemLista * pElem ;
	  
         assert( pLista != NULL ) ;
      

      /* Criar elemento a inerir ap�s */

         CriarElemento( pLista , pValor , &pElem) ;
	
         if ( pElem == NULL )
         {
            printf("Nao ha memoria suficiente\n");
	    exit(-1);
         } /* if */

      /* Encadear o elemento ap�s o elemento */

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
                  
   

   } /* Fim fun��o: LIS  &Inserir elemento ap�s */

/***************************************************************************
*
*  Fun��o: LIS  &Excluir elemento
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

      /* Desencadeia � esquerda */

         if ( pElem->pAnt != NULL )
         {
            pElem->pAnt->pProx   = pElem->pProx ;
            pLista->pElemCorr    = pElem->pAnt ;
         } else {
            pLista->pElemCorr    = pElem->pProx ;
            pLista->pOrigemLista = pLista->pElemCorr ;
         } /* if */

      /* Desencadeia � direita */

         if ( pElem->pProx != NULL )
         {
            pElem->pProx->pAnt = pElem->pAnt ;
         } else{
            pLista->pFimLista = pElem->pAnt ;
         } /* if */

  LiberarElemento( pLista , pElem );
	 
   } /* Fim fun��o: LIS  &Excluir elemento */

/***************************************************************************
*
*  Fun��o: LIS  &Obter refer�ncia para o valor contido no elemento
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
  	
} /* Fim fun��o: LIS  &Obter refer�ncia para o valor contido no elemento */

/***************************************************************************
*
*  Fun��o: LIS  &Ir para o elemento inicial
*  ****/

void LIS_IrInicioLista( LIS_tppLista pLista )
{
	assert(pLista != NULL);

	pLista->pElemCorr = pLista->pOrigemLista ;
} /* Fim fun��o: LIS  &Ir para o elemento inicial */

/***************************************************************************
*
*  Fun��o: LIS  &Ir para o elemento final
*  ****/

void LIS_IrFinalLista(LIS_tppLista pLista)
{
	assert(pLista != NULL);
	
  pLista->pElemCorr = pLista->pFimLista;
} /* Fim fun��o: LIS  &Ir para o elemento final */

/***************************************************************************
*
*  Fun��o: LIS  &Avan�ar elemento
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

  /* Tratar avan�ar para frente */

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

     } /* fim ativa: Tratar avan�ar para frente */

  /* Tratar avan�ar para tr�s */

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

     } /* fim ativa: Tratar avan�ar para tr�s */

  /* Tratar n�o avan�ar */


} /* Fim fun��o: LIS  &Avan�ar elemento */

/***************************************************************************
*
*  Fun��o: LIS  &Procurar elemento contendo valor
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
} /* Fim fun��o: LIS  &Procurar elemento contendo valor */

/***************************************************************************
*
*  Fun��o: LIS  &ObterTamanhoLista
*  ****/

void LIS_ObterTamanhoLista( LIS_tppLista pLista , int* pTamanhoRet) {
	*pTamanhoRet = pLista->numElem;
}
 /* Fim fun��o: LIS  &ObterTamanhoLista */

/***************************************************************************
*
*  Fun��o: LIS  &CorrenteEstaFinal
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
/* Fim fun��o: LIS  &CorrenteEstaFinal*/

/***************************************************************************
*
*  Fun��o: LIS  &CorrenteEstaInicio
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
/* Fim fun��o: LIS  &CorrenteEstaFinal*/

/***********************************************************************
*
*  $FC Fun��o: LIS  &PushCorrente
*
***********************************************************************/

void LIS_PushCorrente(LIS_tppLista pLista) {
	if (pLista->pElemCorr == NULL) {
		printf("Tentativa de push em corrente em lista vazia");
		exit(-1);
	}
	pLista->pBackupCorr = pLista->pElemCorr;
}
/* Fim fun��o: LIS  &PushCorrente*/

/***********************************************************************
*
*  $FC Fun��o: LIS  &PopCorrente
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

/* Fim fun��o: LIS  &PopCorrente*/


/*****  C�digo das fun��es encapsuladas no m�dulo  *****/


/***********************************************************************
*
*  $FC Fun��o: LIS  -Liberar elemento da lista
*
*  $ED Descri��o da fun��o
*     Elimina os espa�os apontados pelo valor do elemento e o
*     pr�prio elemento.
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

   } /* Fim fun��o: LIS  -Liberar elemento da lista */


/***********************************************************************
*
*  $FC Fun��o: LIS  -Criar o elemento
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

   } /* Fim fun��o: LIS  -Criar o elemento */


/***********************************************************************
*
*  $FC Fun��o: LIS  -Limpar a cabe�a da lista
*
***********************************************************************/

   void LimparCabeca( LIS_tppLista pLista )
   {

      pLista->pOrigemLista = NULL ;
      pLista->pFimLista = NULL ;
      pLista->pElemCorr = NULL ;
      pLista->pBackupCorr = NULL;
      pLista->numElem   = 0 ;

   } /* Fim fun��o: LIS  -Limpar a cabe�a da lista */

/********** Fim do m�dulo de implementa��o: LIS  Lista duplamente encadeada **********/
