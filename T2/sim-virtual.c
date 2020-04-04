/***********************************************************************
*	Programa:	Simulador de memória virtual
*	Arquivo:	sim-virtual.c
*	Projeto:	PUC-Rio, Trabalho de INF1019, professor Markus Endler
*	Data de entrega:	21/06/2017
*	Autores:
*		rrc	-	Rafael Rubim Cabral					- matrícula 1511068
*		gab	-	Gabriel de Andrade Busquim	-	matrícula 1510549
*	Arquivo do relatório:	relatorio.txt
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lista.h"

#define PERIODO_ATUALIZACAO 70

typedef enum enPolitica {
	LRU = 1,
		/* Least-Recently-Used */
		
	NRU,
		/* Not-Recently-Used */
		
	NOVO
		/* Nova política */
} tpPolitica;

typedef struct stDescPageframe {
	unsigned char bitM;	//bit que indica se a página correspondente foi modificada
	unsigned char bitR;	//bit que indica se a página correspondente foi referenciada
	unsigned char ultimoAcesso;	//indica o último instante de acesso da página em memória
	int tempoMemoria; //indica em que momento a página entrou na memória
	unsigned char qtdRef; //indica quantas vezes a página foi referenciada em memória
	unsigned char boolEhMapeado;	//indica se a moldura de página é referenciada por uma entrada da tabela
	unsigned int mappedTableEntry;	//indica a entrada da tabela que referencia a moldura de página, se houver alguma
} tpDescPageFrame;

typedef struct stTableEntry {
	unsigned int pageFrame;	//quadro de página correspondente
	unsigned char boolEstaMemoria;	//bit que informa se a a página está mapeada na memória
} tpTableEntry;

typedef struct stInfoAlocacao {
	unsigned char bitOcupado;	//bit que indica se a região da memória em questão está livre ou ocupada
	unsigned int inicio;	//índice do primeiro quadro de página livre/ocupado
	unsigned int ultimo;	//índice do último quadro de página livre/ocupado
} tpInfoAlocacao;

int _debug = 0;
int _time = 0; //simboliza passagem do tempo
tpTableEntry* _tabelaPaginas;
tpDescPageFrame* _descPageFrames;
LIS_tppLista _lstAlocacaoMemoria;
unsigned int _qtdPageFrames;
unsigned int _qtdPags;

/***********************************************************************
*	Função Criar tabela
*
*	Descrição da função
*		Cria e retorna um vetor já inicializado
*		que representará a tabela de páginas.
*		Retorna a quantidade de entradas da tabela por referência.
*		tam_pag deve estar em KB
***********************************************************************/
tpTableEntry* CriarTabela(int tam_pag, unsigned int* qtdPags);

/***********************************************************************
*	Função Criar descritor de page frame
*
*	Descrição da função
*		Cria e retorna um vetor já inicializado que 
*		representará os quadros de página da memória RAM.
*		Retorna a quantidade de molduras de página total por referência.
*		tam_pag deve estar em KB, tam_memoria deve estar em MB
***********************************************************************/
tpDescPageFrame* CriarDescPageFrame(int tam_memoria, int tam_pag, unsigned int* qtdPageFrames);

/***********************************************************************
*	Função Criar lista de alocação de memória
*
*	Descrição da função
*		Cria e retorna uma lista já inicializada que
*		servirá para o controle de alocação de memória livre.
*		Recebe a quantidade de molduras de página total da memória.
***********************************************************************/
LIS_tppLista CriarLstAlocacaoMemoria(unsigned int qtdPageFrames);

/*******************************************************
*	Função: Obter política
*
*	Recebe uma string com uma política de substituição de
*	moldura de páginas e retorna a política	tpPolitica
*	correspondente ou -1, caso não exista.
*******************************************************/
tpPolitica ObterPolitica(char str[]);

/*******************************************************
*	Função: Obter potência de 2
*
*	Recebe um número 'a' e retorna qual é a potência 'p' não
*	negativa de 'a' tal que 2^p = a. Caso 'a' não seja uma
*	potência de 2, retorna -1
*******************************************************/
int ObterPotencia2(int a);

/*******************************************************
*	Função: Obter página
*
*	Recebe um endereço de memória e o tamanho de página da
*	paginação em KB, retornando seu índice de página
*	correspondente
*******************************************************/
unsigned int ObterPagina(unsigned int addr, int tam_pag);

/*******************************************************
*	Função: Levantar Erro
*
*	Recebe uma string com uma mensagem de erro e aborta o
*	programa, escrevendo-a
*******************************************************/
void LevantarErro(const char* msg);

/*******************************************************
*	Função: Acessar Memoria
*
*	Recebe a política de substituição utilizada, o índice
*	de uma página virtual que deseja-se acessar, o modo de
*	acesso (R/W - leitura ou escrita) e referências para
*	contadores de page fault e páginas sujas. A função
*	simula um acesso por memória virtual, carregando a página
*	requerida em uma moldura de página na RAM quando necessário
*	e atualizando as estruturas de dados da tabela de páginas,
*	da alocação de memória e do descritor de molduras de
*	páginas. A função incrementa os contadores de page-fault
*	e página suja, quando esses eventos ocorrerem
*******************************************************/
void AcessarMemoria(tpPolitica politica, unsigned int indicePag, char rw, int* contPageFault, int* contPagSuja);

/*******************************************************
*	Função: Decidir page frame para swap
*
*	Esta função assume que toda a memória RAM está ocupada,
*	decidindo segundo o critério recebido por "politica" qual
*	das molduras de página da RAM deveria ser substituída por
*	uma outra página por ocasião de page-fault.
*******************************************************/
unsigned int DecidirPageFrameSwap(tpPolitica politica);

/*******************************************************
*	Função: Atualizar bits
*
*	Esta função zera os campos bitR de todos os descritores
*	de moldura de página do vetor de descritores global.
*	Também atualiza o campo ultimoAcesso de todos esses
*	descritores.
*******************************************************/
void AtualizarBits();

/*******************************************************
*	Função: Excluir informação de alocação
*
*	Função de exclusão de elemento da lista do tipo
*	tpInfoAlocacao
*******************************************************/
void ExcluirInfoAlocacao(void* elemento);

/*******************************************************
*	Função: Buscar página em info alocação
*
*	Função de comparação de busca em lista que procura em que
*	grupo de páginas ocupadas/livres de um info alocação ela
*	está
*******************************************************/
int BuscarPagInfoAlocacao(void* pElemProc, void* pElemLista);

/*******************************************************
*	Função: Buscar memória livre em info alocação
*
*	Função de comparação de busca em lista que procura um
*	info alocação que é um grupo de páginas livres
*******************************************************/
int BuscarMemLivreInfoAlocacao(void*, void* pElemLista);

/*******************************************************
*	Função: Inserir alocacao memoria
*
*	Informa à lista de alocação de memória que uma moldura
*	de página que estava livre foi alocada.
*******************************************************/
void inserirAlocacaoMemoria(unsigned int pageFrame);

/*******************************************************
*	Função: Remover alocacao memoria
*
*	Informa à lista de alocação de memória que uma moldura
*	de página que estava alocada foi desalocada e agora está
*	livre.
*******************************************************/
void removerAlocacaoMemoria(unsigned int pageFrame);

/*******************************************************
*	Função: Modificar alocacao memoria
*
*	Informa à lista de alocação de memória que uma moldura
*	de página foi alocada/desalocada, se receber boolOcupar
*	como 1/0, respectivamente.
*******************************************************/
void modificarAlocacaoMemoria(unsigned int pageFrame, unsigned char boolOcupar);

/*******************************************************
*	Função: Mensagem Debug
*
*	Recebe uma string e a imprime em stdout, se a global 
*	_debug estiver atribuída
*******************************************************/
void DebugMsg(const char* msg);

int main(int argc, char* argv[])
{
	FILE* myfp;
	int cont_pagFault = 0;
	int cont_pagSuja = 0; 
	tpPolitica politica;
	char rw;
	unsigned int addr, indicePag;
	int tam_pag = atoi(argv[3]);
	int tam_memoria = atoi(argv[4]);
	int linha;
	
	if (argc < 5)
	{
		printf("Uso incorreto do programa! Uso correto: %s (algor de subst) (nome do arquivo) (tamanho da pag em KB) (tamanho da memoria em MB)\n", argv[0]);
		exit(0);
	}
	
	if (argc == 6) {
		_debug = 1;
	}
	
	DebugMsg("PROGRAMA EXECUTADO EM MODO DEBUG");
	
	if ((myfp = fopen (argv[2], "r")) == NULL) 
	{
		LevantarErro("Nao foi possivel abrir arquivo log");
	}
	
	politica = ObterPolitica(argv[1]);
	
	/* INÍCIO Assertivas */
	
	if (politica == -1)
	{
		LevantarErro("Algoritmo passado desconecido. Opções possiveis: LRU, NRU ou NOVO");
	}
	
	if (tam_pag < 8 || tam_pag > 32)
	{
		LevantarErro("Tamanho da pagina deve estar entre 8 e 32 KB");
	}
	
	if (-1 == ObterPotencia2(tam_pag))
	{
		LevantarErro("Tamanho da pagina deve ser uma potencia de 2");
	}
	
	if (tam_memoria < 1 || tam_memoria > 16)
	{
		LevantarErro("Tamanho da memoria deve estar entre 1 e 16 MB");
	}
	
	if(tam_memoria != 1 && tam_memoria != 2 && tam_memoria != 4 && tam_memoria != 8 && tam_memoria != 16)
	{
		LevantarErro("Tamanho da memoria deve ser divisivel pelo tamanho da pagina (i.e. uma potencia de 2 entre 1 e 16 MB)");
	}
	
	/* FIM Assertivas */
	
	_tabelaPaginas = CriarTabela(tam_pag, &_qtdPags);
	_descPageFrames = CriarDescPageFrame(tam_memoria, tam_pag, &_qtdPageFrames);
	_lstAlocacaoMemoria = CriarLstAlocacaoMemoria(_qtdPageFrames);
	
	printf("Executando o simulador...\n");
	linha = 1;
	while (fscanf(myfp, "%x %c ", &addr, &rw) == 2)
	{
		if (_time % PERIODO_ATUALIZACAO == 0) {
			AtualizarBits();
		}
		indicePag = ObterPagina(addr, tam_pag);
		AcessarMemoria(politica, indicePag, rw, &cont_pagFault, &cont_pagSuja);
		if (_debug == 1) {
			unsigned int i;
			printf("\nNova linha: %d\n\n", linha);
			linha++;
			for (i = 0; i < _qtdPageFrames; i++) {
				printf("PF %u: M:%d\tR:%d\tEnv:%d\tMap:%d\tPte:%x\n", i + 1, _descPageFrames[i].bitM, _descPageFrames[i].bitR, _descPageFrames[i].ultimoAcesso, _descPageFrames[i].boolEhMapeado, _descPageFrames[i].mappedTableEntry);
			}
		}
		_time++;
	}
	     
	printf("Arquivo de entrada: %s\n", argv[2]);     
	printf("Tamanho da memoria fisica: %d MB\n", tam_memoria);     
	printf("Tamanho das páginas: %d KB\n", tam_pag);     
	printf("Alg de substituição: %s\n", argv[1]);     
	printf("Numero de Faltas de Páginas: %d\n", cont_pagFault);     
	printf("Numero de Paginas escritas: %d\n", cont_pagSuja);
	
	fclose(myfp);
	free(_tabelaPaginas);
	free(_descPageFrames);
	LIS_DestruirLista(_lstAlocacaoMemoria);
	return 0;
}

unsigned int ObterPagina(unsigned int addr, int tam_pag)
{
	int potencia2 = ObterPotencia2(tam_pag);
	potencia2 += 10;
	if (potencia2 == -1) {
		LevantarErro("Funcao ObterPagina so trabalha com paginas cujo tamanho eh potencia de 2");
	}
	return addr >> potencia2;
}

int ObterPotencia2(int a) {
	int recursao;
	if (a == 1) {
		return 0;
	}
	if (a <= 0 || (a % 2) != 0) {
		return -1;
	}
	recursao = ObterPotencia2(a / 2);
	if (recursao == -1) {
		return -1;
	}
	return recursao + 1;
}

tpPolitica ObterPolitica(char str[])
{
	if (strcmp(str,"LRU") == 0 || strcmp(str,"lru") == 0 )
		return LRU;
	else if (strcmp(str,"NRU") == 0 || strcmp(str,"nru") == 0 )
		return NRU;
	else if (strcmp(str,"NOVO") == 0 || strcmp(str,"novo") == 0 )
		return NOVO;
	return -1;
}

void LevantarErro(const char* msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

tpTableEntry* CriarTabela(int tam_pag, unsigned int* qtdPags)
{
	int pot = ObterPotencia2(tam_pag);
	unsigned int i, espacoEnderecamento;
	espacoEnderecamento = pow(2, (22-pot));
	tpTableEntry* tabelaPagina = (tpTableEntry*) malloc(espacoEnderecamento * sizeof(tpTableEntry));
	if(tabelaPagina == NULL)
	{
		LevantarErro("Memoria Insufuciente");
	}
	
	for(i = 0; i < espacoEnderecamento; i++)
	{
		tabelaPagina[i].pageFrame = 0;
		tabelaPagina[i].boolEstaMemoria = 0;
	}
	
	*qtdPags = espacoEnderecamento;
	return tabelaPagina;
}

tpDescPageFrame* CriarDescPageFrame(int tam_memoria, int tam_pag, unsigned int* qtdPageFrames)
{
	int pot_pag = ObterPotencia2(tam_pag);
	int pot_mem = ObterPotencia2(tam_memoria);
	int exp_numQuadros = pot_mem - pot_pag + 10;
	unsigned int total_numQuadros = pow(2, exp_numQuadros);
	unsigned int i;
	tpDescPageFrame* vetorPageFrame=(tpDescPageFrame*) malloc (total_numQuadros * sizeof(tpDescPageFrame));
	if(vetorPageFrame == NULL)
	{
		LevantarErro("Memória Insufuciente");
	}
	*qtdPageFrames = total_numQuadros;
	for(i = 0; i < total_numQuadros; i++)
	{
		vetorPageFrame[i].bitM = 0;
		vetorPageFrame[i].bitR = 0;
		vetorPageFrame[i].ultimoAcesso = 0;
		vetorPageFrame[i].boolEhMapeado = 0;
		vetorPageFrame[i].mappedTableEntry = 0;
	}
	return vetorPageFrame;
}

LIS_tppLista CriarLstAlocacaoMemoria(unsigned int qtdPageFrames) {
	LIS_tppLista lst;
	tpInfoAlocacao* tudoLivre;
	LIS_CriarLista(&lst, ExcluirInfoAlocacao);
	tudoLivre = (tpInfoAlocacao*) malloc(sizeof(tpInfoAlocacao));
	if (tudoLivre == NULL) {
		LevantarErro("Falta de memória");
	}
	tudoLivre->bitOcupado = 0;
	tudoLivre->inicio = 0;
	tudoLivre->ultimo = (unsigned int) qtdPageFrames - 1;
	LIS_InserirElementoApos(lst, tudoLivre);
	return lst;
}

void ExcluirInfoAlocacao(void* elemento) {
	free((tpInfoAlocacao*) elemento);
}

int BuscarPagInfoAlocacao(void* pElemProc, void* pElemLista) {
	unsigned int pagProcurada = *(unsigned int*) pElemProc;
	tpInfoAlocacao* possivelGrupo = (tpInfoAlocacao*) pElemLista;
	if (pagProcurada >= possivelGrupo->inicio && pagProcurada <= possivelGrupo->ultimo) {
		return 1;
	}
	return 0;
}

int BuscarMemLivreInfoAlocacao(void* pElemProc, void* pElemLista) {
	tpInfoAlocacao* possivelGrupo = (tpInfoAlocacao*) pElemLista;
	if (possivelGrupo->bitOcupado == 0) {
		return 1;
	}
	return 0;
}

void modificarAlocacaoMemoria(unsigned int pageFrame, unsigned char boolOcupar) {
	int encontrou;
	tpInfoAlocacao* grupoPags;
	unsigned int primeiroDoNovo;
	unsigned int ultimoDoNovo;
	
	encontrou = LIS_ProcurarValor(_lstAlocacaoMemoria, &pageFrame, BuscarPagInfoAlocacao);
	if (encontrou == 0) {
		LevantarErro("Pagina nao foi encontrada em estrutura de desalocacao de memoria");
	}
	LIS_ObterValor(_lstAlocacaoMemoria, (void**) &grupoPags);
	
	/* Assertiva: se a página modificada já estava ocupada/livre */
	if (grupoPags->bitOcupado == boolOcupar) {
		if (boolOcupar == 1) {
			LevantarErro("Pagina alocada ja esta ocupada");
		}
		else {
			LevantarErro("Pagina desalocada ja esta livre");
		}
	}
	
	/* Um novo grupo será criado, que contém a página alocada/desalocada */
	primeiroDoNovo = pageFrame;
	ultimoDoNovo = pageFrame;
	
	/* Se a página estava no início do grupo e se havia algum grupo antes */
	if (grupoPags->inicio == pageFrame && 1 != LIS_CorrenteEstaInicio(_lstAlocacaoMemoria)) {
		int precisaVoltar;
		tpInfoAlocacao* tempGrupo;
		/* Obter grupo anterior, modificar primeiro do novo */
		LIS_AvancarElementoCorrente(_lstAlocacaoMemoria, -1);
		LIS_ObterValor(_lstAlocacaoMemoria, (void**) &tempGrupo);
		primeiroDoNovo = tempGrupo->inicio;
		/* Excluir grupo anterior, voltar corrente para grupoPags se necessário */
		precisaVoltar = 0;
		if (1 != LIS_CorrenteEstaInicio(_lstAlocacaoMemoria)) {
			precisaVoltar = 1;
		}
		LIS_ExcluirElemento(_lstAlocacaoMemoria);
		if (precisaVoltar == 1) {
			LIS_AvancarElementoCorrente(_lstAlocacaoMemoria, 1);
		}
	}
	/* Se a página não estava no início do grupo */
	else if (grupoPags->inicio != pageFrame) {
		/* Corrente está em grupoPags. Criar grupo antes, do início até
		pageFrame - 1, com o mesmo bit de ocupação */
		tpInfoAlocacao* novoGrupoAntes = (tpInfoAlocacao*) malloc(sizeof(tpInfoAlocacao));
		if (novoGrupoAntes == NULL) {
			LevantarErro("Falta de memória");
		}
		novoGrupoAntes->inicio = grupoPags->inicio;
		novoGrupoAntes->ultimo = pageFrame - 1;
		novoGrupoAntes->bitOcupado = grupoPags->bitOcupado;
		LIS_InserirElementoAntes(_lstAlocacaoMemoria, novoGrupoAntes);
		/* Voltar corrente para grupoPags */
		LIS_AvancarElementoCorrente(_lstAlocacaoMemoria, 1);
	}
	
	/* Se a página estava no final do grupo e se havia algum grupo depois */
	if (grupoPags->ultimo == pageFrame && 1 != LIS_CorrenteEstaFinal(_lstAlocacaoMemoria)) {
		tpInfoAlocacao* tempGrupo;
		/* Obter grupo depois, modificar último do novo */
		LIS_AvancarElementoCorrente(_lstAlocacaoMemoria, 1);
		LIS_ObterValor(_lstAlocacaoMemoria, (void**) &tempGrupo);
		ultimoDoNovo = tempGrupo->ultimo;
		/* Excluir grupo depois, corrente volta para grupoPags sozinho */
		LIS_ExcluirElemento(_lstAlocacaoMemoria);
	}
	/* Se a página não estava no final do grupo */
	else if (grupoPags->ultimo != pageFrame) {
		/* Corrente está em grupoPags. Criar grupo depois, de pageFrame + 1 até
		o final, com o mesmo bit de ocupação */
		tpInfoAlocacao* novoGrupoDepois = (tpInfoAlocacao*) malloc(sizeof(tpInfoAlocacao));
		if (novoGrupoDepois == NULL) {
			LevantarErro("Falta de memória");
		}
		novoGrupoDepois->inicio = pageFrame + 1;
		novoGrupoDepois->ultimo = grupoPags->ultimo;
		novoGrupoDepois->bitOcupado = grupoPags->bitOcupado;
		LIS_InserirElementoApos(_lstAlocacaoMemoria, novoGrupoDepois);
		/* Voltar corrente para grupoPags */
		LIS_AvancarElementoCorrente(_lstAlocacaoMemoria, -1);
	}
	
	/* Deve-se excluir grupoPags e criar novo grupo que contém
	a página alocada/desalocada no lugar. Na prática, pode-se alterar grupoPags */
	grupoPags->inicio = primeiroDoNovo;
	grupoPags->ultimo = ultimoDoNovo;
	grupoPags->bitOcupado = boolOcupar;
}

void inserirAlocacaoMemoria(unsigned int pageFrame) {
	modificarAlocacaoMemoria(pageFrame, 1);
}

void removerAlocacaoMemoria(unsigned int pageFrame) {
	modificarAlocacaoMemoria(pageFrame, 0);
}

void AtualizarBits()
{
	unsigned int i;
	for (i = 0; i < _qtdPageFrames; i++)
	{
		_descPageFrames[i].ultimoAcesso >>= 1;
		_descPageFrames[i].bitR = 0;
	}
}

void AcessarMemoria(tpPolitica politica, unsigned int indicePag, char rw, int* contPageFault, int* contPagSuja) {
	unsigned int pageFrame;
	
	/* Página acessada não está na memória: page-fault */
	if (_tabelaPaginas[indicePag].boolEstaMemoria == 0) {
		int temMemLivre;
		
		/* Incrementar contador de page-faults */
		*contPageFault += 1;
		DebugMsg("contPageFault++");
		
		temMemLivre = LIS_ProcurarValor(_lstAlocacaoMemoria, NULL, BuscarMemLivreInfoAlocacao);
		/* Se houver memória livre, alocá-la */
		if (temMemLivre == 1) {
			tpInfoAlocacao* grupoLivre;
			LIS_ObterValor(_lstAlocacaoMemoria, (void*) &grupoLivre);
			pageFrame = grupoLivre->inicio;
			inserirAlocacaoMemoria(pageFrame);
		}
		/* Se não houver memória livre, decidir qual memória será substituída e fazer swap */
		else {
			unsigned int tableEntryAntiga;
			pageFrame = DecidirPageFrameSwap(politica);
			/* Página escolhida está suja? */
			if (_descPageFrames[pageFrame].bitM == 1) {
				/* Incrementar contador de páginas sujas */
				*contPagSuja += 1;
				DebugMsg("contPagSuja++");
			}
			/* Conteúdo de moldura será substituído, guardar informação necessária */
			/* Atualizar table entry que apontava para a moldura que será substituída,
			"avisando-a" que a memória virtual agora está em disco, e não na RAM */
			tableEntryAntiga = _descPageFrames[pageFrame].mappedTableEntry;
			/* Nossa implementação não considera o disco, portanto na prática não há
			inode para o qual apontar. Quando não está em disco, a entrada da tabela
			de páginas aponta para 0 */
			_tabelaPaginas[tableEntryAntiga].boolEstaMemoria = 0;
			_tabelaPaginas[tableEntryAntiga].pageFrame = 0;
		}
		/* Moldura foi ocupada, guardar informação necessária */
		/* Atualizar bits de referência/escrita */
		_descPageFrames[pageFrame].bitR = 1;
		if (rw == 'W') {
			_descPageFrames[pageFrame].bitM = 1;
		}
		/* Após page-fault, atualizar campos da moldura de página */
		_descPageFrames[pageFrame].boolEhMapeado = 1;
		_descPageFrames[pageFrame].mappedTableEntry = indicePag;
		_descPageFrames[pageFrame].ultimoAcesso = 128;
		_descPageFrames[pageFrame].tempoMemoria = _time;
		_descPageFrames[pageFrame].qtdRef = 1;
		/* Atualizar também tabela de páginas */
		_tabelaPaginas[indicePag].boolEstaMemoria = 1;
		_tabelaPaginas[indicePag].pageFrame = pageFrame;
	}
	/* Página acessada está na memória: pegar moldura correspondente na tabela */
	else {
		pageFrame = _tabelaPaginas[indicePag].pageFrame;
		/* Incrementar contador de referências à página */
		_descPageFrames[pageFrame].qtdRef += 1;
		/* Atualizar bit de último acesso */
		_descPageFrames[pageFrame].ultimoAcesso += 128;
		/* Atualizar bits de referência/escrita */
		_descPageFrames[pageFrame].bitR = 1;
		if (rw == 'W') {
			_descPageFrames[pageFrame].bitM = 1;
		}
	}	
}

unsigned int DecidirPageFrameSwap(tpPolitica politica)
{
	unsigned int i;
	tpDescPageFrame pageFrameAtual;
	unsigned int pageFrameMenorTempo;
	unsigned char boolEncontrouNaoRM = 0;
	unsigned int pageFrameNaoRM;
	unsigned char boolEncontrouRNaoM = 0;
	unsigned int pageFrameRNaoM;
	unsigned char boolEncontrouRM = 0;
	unsigned int pageFrameRM;
	unsigned char contadorReferencia;
	float freqReferencia;
	unsigned int pageFrameMenorFreq;
	
	switch(politica)
	{
		case NRU:
			for (i = 0; i < _qtdPageFrames; i++)
			{
				pageFrameAtual = _descPageFrames[i];
				if(pageFrameAtual.bitR == 0 && pageFrameAtual.bitM == 0) {
					return i;
				}
				else if (boolEncontrouNaoRM == 0 && pageFrameAtual.bitR == 0 && pageFrameAtual.bitM == 1) {
					boolEncontrouNaoRM = 1;
					pageFrameNaoRM = i;
				}
				else if (boolEncontrouRNaoM == 0 && pageFrameAtual.bitR == 1 && pageFrameAtual.bitM == 0) {
					boolEncontrouRNaoM = 1;
					pageFrameRNaoM = i;
				}
				else if (boolEncontrouRM == 0) {
					boolEncontrouRM = 1;
					pageFrameRM = i;
				}
			}
			if (boolEncontrouNaoRM == 1) {
				return pageFrameNaoRM;
			}
			else if (boolEncontrouRNaoM == 1) {
				return pageFrameRNaoM;
			}
			else if (boolEncontrouRM == 1) {
				return pageFrameRM;
			}
			else {
				LevantarErro("Inconsistencia");
				return 0;
			}
			break;
		case LRU:
			pageFrameMenorTempo = 0;
			contadorReferencia = _descPageFrames[0].ultimoAcesso;
			for (i = 1; i < _qtdPageFrames; i++)
			{
				pageFrameAtual = _descPageFrames[i];
				if (pageFrameAtual.ultimoAcesso < contadorReferencia)
				{
					contadorReferencia = pageFrameAtual.ultimoAcesso;
					pageFrameMenorTempo = i;
				}
			}
			return pageFrameMenorTempo;
			break;
		case NOVO:
			{
				int tempoNaMem = _time - _descPageFrames[0].tempoMemoria;
				freqReferencia = (float) (int) _descPageFrames[0].qtdRef;
				if (tempoNaMem == 0) {
					LevantarErro("Inconsistencia");
				}
				freqReferencia /= tempoNaMem;
				pageFrameMenorFreq = 0;
			}
			for (i = 1; i < _qtdPageFrames; i++) {
				int tempoNaMem;
				float freqAtual;
				pageFrameAtual = _descPageFrames[i];
				tempoNaMem = _time - pageFrameAtual.tempoMemoria;
				freqAtual = (float) (int) pageFrameAtual.qtdRef;
				if (tempoNaMem == 0) {
					LevantarErro("Inconsistencia");
				}
				freqAtual /= tempoNaMem;
				if (freqAtual < freqReferencia) {
					freqReferencia = freqAtual;
					pageFrameMenorFreq = i;
				}
			}
			return pageFrameMenorFreq;
			break;
		default:
			LevantarErro("Politica de swap desconhecida");
	}
	return 0;
}

void DebugMsg(const char* msg) {
	if (_debug == 1) {
		printf("DEBUG: %s\n", msg);
	}
}
