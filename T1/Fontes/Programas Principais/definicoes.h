/*********************************************************************************************
*	Módulo de definições: definicoes.h
*	Letras identificadoras: DEF
*	Descrição:
*		Define estrura de processos e seus tipos de política de escalonamento.
*		Não possui módulo de implementação.
*	Autores:	rrc	-	gab
*	Histórico de desenvolvimento:
*		Versão	-	Data	-	Descrição
*		1.0			-	19/04	-	criação completa do módulo
*********************************************************************************************/

#pragma once

/*********************************************************************************************
*			Definições de tipos de Dados
*/

/*********************************************************************************************
*	Tipo enumerado: DEF_tpPoliticaEsc
*	Descrição:
*		Representa uma entre 3 políticas de escalonamento de um processo.
*********************************************************************************************/
enum DEF_enPoliticaEsc{
	REAL_TIME		=	0,
	PRIORIDADE 	=	1,
	ROUND_ROBIN	=	2
};

typedef enum DEF_enPoliticaEsc DEF_tpPoliticaEsc;

/*********************************************************************************************
*	Tipo estruturado: DEF_tpProcesso
*	Descrição:
*		Representa um processo cuja execução será controlada por um escalonador.
*********************************************************************************************/
struct DEF_stProcesso {
	int pid;
		/* ID único do processo */
		
	char nomeArquivo[201];
		/* Nome do arquivo executável do processo (assume-se que o path do arquivo é o diretório
		* corrente) */
	
	DEF_tpPoliticaEsc politica;
		/* Tipo de política que o escalonador usará para gerenciar processo */
	
	int prioridade;
		/* Somente para processos de política tipo PRIORIDADE. Representa a prioridade, de 1 a 7,
		*	de escalonamento do processo */
	
	int tempoInicio;
		/* Somente para processos de política tipo REAL_TIME. Representa, em segundos, o momento de
		* resumo do processo em relação a cada início de um minuto */
	
	int tempoDuracao;
		/* Somente para processos de política tipo REAL_TIME. Representa, em segundos, por quanto
		tempo o processo executará após ser resumido antes que pare novamente */
};

typedef struct DEF_stProcesso DEF_tpProcesso;
