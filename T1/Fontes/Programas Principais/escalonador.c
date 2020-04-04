/*********************************************************************************************
*	Módulo de implementação: escalonador.c
*	Letras identificadoras: ESC
*	Descrição:
*		Implementa um programa que simula um escalonador, em conjunto com o programa interpretador.
*		O escalonador gerenciará a execução e troca de contexto de processos passados pelo
*		interpretador.
*		Não possui módulo de definição.
*	Autores:	rrc	-	gab
*	Histórico de desenvolvimento:
*		Versão	-	Data	-	Descrição
*		1.0			-	19/04	-	início da implementação
*		1.0			-	25/04	-	implementação das funções e suporte a sinais
*********************************************************************************************/

#include "definicoes.h"
#include "lstprio.h"
#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <math.h>

#define EVER ;;
#define TEMPOCOMA 40
	/* Quando o escalonador não está executando programas e não precisa acordar de tempos em tempos,
	de TEMPOCOMA em TEMPOCOMA segundos ele dá um sleep. */
#define _DEBUG 0
	/* Quando se quer que algumas mensagens para ajudar no debug sejam impressas, _DEBUG = 1 */
#define MILISSEGS_RR 500000
	/* Tempo em milissegundos de duração dos processos Round-Robin. (Os de prioridade são o dobro,
	triplo etc. deste valor) */
#define DEBUG_TEMPOINICIAL 0
	/* Tempo em segundos que o usuário gostaria que começasse o segundo atual caso _DEBUG seja 1 */

/*********************************************************************************************
*			Definições de Dados Estáticos
*/
int _pipeProcessos;
	/* Pipe através da qual o escalonador recebe novos processos */

DEF_tpProcesso* _processoCorrente;
	/* Processo que está atualmente em execução, contexto atual */

LIS_tppLista _lstRoundRobin;
	/* Lista de processos que rodarão em round-robin. Isso inclui processos da política de prioridades */

LSTP_tppLstPrio _lstRealTimeProx;
	/* Lista de prioridades de processos real-time controlados pelo escalonador a serem
	* executados até o fim do minuto atual */
	
LSTP_tppLstPrio _lstRealTimePast;
	/* Lista de prioridades de processos real-time controlados pelo escalonador já executados
	* (executando inclusive) desde o início do minuto atual */

int _boolComa;
	/* Booleano. Assume o valor 1 quando o escalonador não tem horário marcado para voltar a acordar,
	então fica num sleep eterno até qe precise escalonar alguma coisa. */

long _tempoSono;
	/* Valor em microsegundos que o programa deverá dormir */

struct timespec _tempoUltimoSono;
	/* Guarda o tempo em que começou o último sono do escalonador pra calcular o tempo que ele durou.
	Isso serve pro caso de ele ter sido acordado prematuramente e precisar voltar a dormir, pois é
	necessário saber quanto tempo mais de sono falta pra acabar */

struct timespec _tempoInicio;
	/* Tempo de inicio de execução do programa, só usado com _DEBUG definido */

/*********************************************************************************************
*			Definições de Funções Estáticas
*/

static int CompararPrioridadeRT(void* pElem1, void* pElem2);

static void DestruirPProcesso(void* pElem);

static int CompararPid(void* pElemProc, void* pElemLista);

static void HandlerNovoPid();

static void OrganizarTrocaContexto();

static void TrocarContexto(DEF_tpProcesso* novoProcesso);

static void TratarProcessoEncerrado();

static void LevantarErro(const char* msg);

static void DebugMsg(const char* msg);

static long ObterMilissegCorr();

static void ConfigurarEscalonador();

static void TratarSinal(int sig);

static void MascararSinais();

static void DesmascararSinais();

static int BuscarConflitosLstRT(void* novoProcesso, void* processoLst);

static int ConferirValidadeRT(DEF_tpProcesso* processo);

static int VerificarAlgumRR();

static int VerificarAlgumRT();

static void AvancarCorrenteCircular(LIS_tppLista lst);

static void RegredirCorrenteCircular(LIS_tppLista lst);

static struct timespec ObterTempo();

static long ObterTempoPassado(struct timespec inicial);

static void RefrescarListasRealTime();

static void PedirEntrarComa();

static void PedirSairComa();

static void PedirDormir(long tempo);

static void PedirAcordar();

static void ImprimirProcesso(DEF_tpProcesso* p, const char* msg);

/*********************************************************************************************
*			Função Principal
*/

int main(int argc, char** argv) {
		/* Escalonador recebe como parâmetro o número de descrição da saída da pipe criada pelo interpretador */
		
	int boolImprimirDormir = 1;
	
	if (argc != 2) {
		LevantarErro("Uso do programa: ./escalonador <num_pipe>");
	}
	
	/* Inicializar variáveis globais: */
	_processoCorrente = NULL;
	
	_pipeProcessos = atoi(argv[1]);
	
	_lstRealTimeProx = LSTP_LstPrioCriar(CompararPrioridadeRT, DestruirPProcesso);
	_lstRealTimePast = LSTP_LstPrioCriar(CompararPrioridadeRT, DestruirPProcesso);
	LIS_CriarLista(&_lstRoundRobin, DestruirPProcesso);
	
	_tempoSono = 0;
	_boolComa = 1;
	
	ConfigurarEscalonador();
	
	if (_DEBUG == 1) {
		_tempoInicio = ObterTempo();
	}
	
	if (_DEBUG == 1) {
		printf("ESC DEBUG: Escalonador comecando no segundo do minuto atual: %.4f\n\n", ObterMilissegCorr() / 1e6);
		fflush(stdout);
	}
	
	printf("ESC: Escalonador iniciado. Estado incial: coma\n\n");
	fflush(stdout);
	
	/* Roda em loop dormindo, esperando sinais ou acordando de tempos em tempos por si próprio */
	for(EVER) {
		/* Se o esclonador não está em coma, ele deve acordar em _tempoSono microsegundos */
		if (_boolComa == 0) {
			
			if (boolImprimirDormir == 1 && _tempoSono > 0) {
				printf("ESC: Escalonador dormira por: %.4f segundos\n\n", _tempoSono/ 1.0e6);
				fflush(stdout);
			}
			boolImprimirDormir = 1;
			
			/* Salvar quando teoricamente comecou a dormir */
			_tempoUltimoSono = ObterTempo();
			
			/* Dormir antes de acordar */
			usleep(_tempoSono);
			
			MascararSinais();
			{
				long tempoPassado = ObterTempoPassado(_tempoUltimoSono);
				/* Se o sleep dormiu tudo */
				if (tempoPassado >= _tempoSono) {
					MascararSinais();
						OrganizarTrocaContexto();
					DesmascararSinais();
				}
				/* Se o sleep foi interrompido */
				else {
					_tempoSono = _tempoSono - tempoPassado;
					/* Não imprimir que vai dormir de novo */
					boolImprimirDormir = 0;
				}
			}
			DesmascararSinais();
		}
		/* Escalonador em coma: nenhum programa para escalonar */
		else {
			DebugMsg("Escalonador em coma");
			sleep(TEMPOCOMA);
		}
	}
	return 0;
}

/*********************************************************************************************
*			Implementações de Funções Estáticas
*/

/*********************************************************************************************
*	Função: comparar prioridade real-time
*	Descrição:
*		Função que compara dois processos Real-Time e retorna 1 caso o primeiro ocorra antes do
*		segundo, 0 caso contrário. Serve para criar uma lista cujas prioridades sejam processos
*		
*********************************************************************************************/
static int CompararPrioridadeRT(void* pElem1, void* pElem2) {
	DEF_tpProcesso* pProcesso1 = (DEF_tpProcesso*) pElem1;
	DEF_tpProcesso* pProcesso2 = (DEF_tpProcesso*) pElem2;
	if (pProcesso1->tempoInicio < pProcesso2->tempoInicio) {
		return 1;
	}
	return 0;
}

/*********************************************************************************************
*	Função: destruir ponteiro de processo
*	Descrição:
*		Recebe um ponteiro de um DEF_tpProcesso em formato genérico e o libera na
*		memória. É a função padrão de destruição de uma lista de processos.
*********************************************************************************************/
static void DestruirPProcesso(void* pElem) {
	DEF_tpProcesso* pProcesso = (DEF_tpProcesso*) pElem;
	free(pProcesso);
}

/*********************************************************************************************
*	Função: comparar pid
*	Descrição:
*		Função que compara um pid e um processo e retorna 1 caso o processo possua este pid, ou
*		0 caso contrário. Serve para buscar um pid numa lista de processos.
*********************************************************************************************/
static int CompararPid(void* pElemProc, void* pElemLista) {
	int* pPidProc = (int*) pElemProc;
	DEF_tpProcesso* pProcessoLista = (DEF_tpProcesso*) pElemLista;
	if (pProcessoLista->pid == *pPidProc) {
		return 1;
	}
	else {
		return 0;
	}
}

/*********************************************************************************************
*	Função: handler novo pid
*	Descrição:
*		Função que trata a criação de um novo processo, adicionando-o ao banco de dados do
*		escalonador. Trata também a possível necessidade de uma troca de contexto imediata.
*********************************************************************************************/
static void HandlerNovoPid() {
	DEF_tpProcesso* novoProcesso;
	
	/* Ler qual é o novo processo */
	novoProcesso = (DEF_tpProcesso*) malloc(sizeof(DEF_tpProcesso));
	if (novoProcesso == NULL) {
		LevantarErro("Erro em malloc [1]");
	}
	if (sizeof(DEF_tpProcesso) != read(_pipeProcessos, novoProcesso, sizeof(DEF_tpProcesso))) {
		LevantarErro("Nao ha processo no pipe a ser lido");
	}
	
	/* Imprime dados do processo que tentará ser executado */
	ImprimirProcesso(novoProcesso, "Novo processo passado ao escalonador");
	
	if (_DEBUG == 1) {
		long milissegundo;
		milissegundo = ObterMilissegCorr();
		printf("ESC DEBUG: Segundo do minuto atual: %.4f\n\n", milissegundo / 1e6);
		fflush(stdout);
	}
	
	/* Se o novo processo for Real-Time */
	if (novoProcesso->politica == REAL_TIME) {
		long milissegundo = ObterMilissegCorr();
			/* milissegundo de 0 a 60 * 1e6 do minuto atual */
		
		/* Se os tempos de inicio e duracao forem invalidos, uma mensagem
		será impressa (pela função ConferirValidadeRT) e nada mais acontecerá */
		if (-1 == ConferirValidadeRT(novoProcesso)) {
			/* Não fazer nada */
			return;
		}
		/* Caso os tempos sejam válidos, pode-se inseri-lo em uma das listas Real-Time */
		/* Se o tempo de inicio ainda esta por vir, inseri-lo entre os próximos */
		if ((double) novoProcesso->tempoInicio > ((double) milissegundo / 1e6)) {
			LSTP_LstPrioInserir(_lstRealTimeProx, novoProcesso);
			DebugMsg("Novo processo Real-Time inserido na lista de proximos");
			/* Pedir troca de contexto */
			PedirAcordar();
			PedirSairComa();
			return;
		}
		/* Se o tempo de inicio já passou */
		else {
			/* Se era pra estar executando real-time */
			if ((double) (novoProcesso->tempoInicio + novoProcesso->tempoDuracao) > ((double) milissegundo) / 1e6) {
				/* Inseri-lo entre os próximos */
				LSTP_LstPrioInserir(_lstRealTimeProx, novoProcesso);
				DebugMsg("Novo processo Real-Time exige execucao imediata");
				/* Pedir troca de contexto */
				PedirAcordar();
				PedirSairComa();
				return;
			}
			/* Se não precisaria estar executando mais */
			else {
				/* Inseri-lo entre os que já passaram */
				LSTP_LstPrioInserir(_lstRealTimePast, novoProcesso);
				DebugMsg("Novo processo Real-Time inserido na lista de passados");
				/* Pedir troca de contexto */
				PedirAcordar();
				PedirSairComa();
				return;
			}
		}
	}
	/* Se o novo processo for Round-Robin ou Prioridade */
	else if (novoProcesso->politica == ROUND_ROBIN || novoProcesso->politica == PRIORIDADE) {
		
		/* Se há um processo atualmente rodando e ele tem política Prioridade/Round-Robin,
		então deve ser interrompido caso a nova prioridade seja maior */
		if (_processoCorrente != NULL && (_processoCorrente->politica == PRIORIDADE || _processoCorrente->politica == ROUND_ROBIN)) {
			/* Booleanos com alguns casos */
			int boolNovoTemPrioridade = _processoCorrente->politica == ROUND_ROBIN && novoProcesso->politica == PRIORIDADE;
			int boolAmbosPrioridade = _processoCorrente->politica == PRIORIDADE && novoProcesso->politica == PRIORIDADE;
			int boolNovoTemPrioridadeSuperior = novoProcesso->prioridade < _processoCorrente->prioridade;
			/* Se o novo processo é superior ao corrente */
			if (boolNovoTemPrioridade || (boolAmbosPrioridade && boolNovoTemPrioridadeSuperior)) {
			
				/* Inserir novo processo como próximo elemento da lista de round-robin */
				LIS_InserirElementoApos(_lstRoundRobin, novoProcesso);
				/* Correção: voltar corrente da lista para original */
				RegredirCorrenteCircular(_lstRoundRobin);
				DebugMsg("Novo processo prioridade possui prioridade superior ao processo atual e o interrompera");
				/* Pedir troca de contexto imediata */
				PedirAcordar();
				PedirSairComa();
				return;
			}
		}
		/* Se não há processos em execução, trocar contexto imediatamente */
		else if (_processoCorrente == NULL) {
			/* Assertiva */
			if (0 != VerificarAlgumRR()) {
				LevantarErro("Nao ha processo em execucao mas ha processos Round-Robin na lista para serem executados");
			}
			/* Inserir novo processo como próximo elemento da lista de round-robin */
			LIS_InserirElementoApos(_lstRoundRobin, novoProcesso);
			/* Correção: voltar corrente da lista para original */
			RegredirCorrenteCircular(_lstRoundRobin);
			DebugMsg("Novo processo eh o primeiro processo prioridade/round-robin da lista e rodara imediatamente");
			/* Pedir troca de contexto imediata */
			PedirAcordar();
			PedirSairComa();
			return;
		}
			/* Se o programa chegou aqui, então existe um processo corrente e o novo processo não o interromperá,
			pois o corrente é Real-Time ou possui prioridade superior ou igual ao novo processo */
		{
			int qtdRR;
			/* Obter quantidade de Round-Robin existentes */
			LIS_ObterTamanhoLista(_lstRoundRobin, &qtdRR);
			
			/* Se ainda não há processos Round-Robin */
			if (qtdRR == 0) {
				/* Inserir novo processo como primeiro elemento da lista de round-robin */
				LIS_InserirElementoApos(_lstRoundRobin, novoProcesso);
				DebugMsg("Novo processo eh o primeiro processo prioridade/round-robin da lista e rodara apos o(s) real-time(s) atual(is) finzalizar");
				
				/* Assertiva: Se escalonador estava em coma */
				if (_boolComa == 1) {
					LevantarErro("Escalonador esta em coma com um processo executando que nao eh Round-Robin");
				}
				return;
			}
			/* Se há processos Round-Robin, inserir novo processo logo antes do primeiro processo de prioridade
			inferior à dele */
			else {
				DEF_tpProcesso* correnteInicial,* tempProcesso;
				int boolInseri = 0;
				/* Obter nivel de prioridade do novo processo (Round-Robin é tratado como 8) */
				int nivelPrioridade = 8;
				if (novoProcesso->politica == PRIORIDADE) {
					nivelPrioridade = novoProcesso->prioridade;
				}
				/* Obter processo corrente da lista e salvá-lo */
				LIS_ObterValor(_lstRoundRobin, (void**)&correnteInicial);
				/* Salvar corrente da lista para restaurá-lo mais abaixo */
				LIS_PushCorrente(_lstRoundRobin);
				/* Percorrer lista a partir de seu corrente, procurando onde inserir o novo */
				tempProcesso = correnteInicial;
				do {
					/* Obter nivel de prioridade de cada processo da lista (Round-Robin é tratado como 8) */
					int nivelPrioridadeTemp = 8;
					if (tempProcesso->politica == PRIORIDADE) {
						nivelPrioridadeTemp = tempProcesso->prioridade;
					}
					
					/* Testar se o nível de prioridade do processo da lista é inferior ao do novo.
					Se sim, encontrado onde inserir o novo! */
					if (nivelPrioridadeTemp > nivelPrioridade) {
						/* Se eu ainda estou na primeira iteração, então devo inserir o novo processo
						à frente do tempProcesso, pois ele deve ser o próximo a executar */
						if (tempProcesso == correnteInicial) {
							/* Inserir novo processo como próximo elemento da lista de round-robin */
							LIS_InserirElementoApos(_lstRoundRobin, novoProcesso);
							boolInseri = 1;
							/* Parar de percorrer */
							break;
						}
						/* Em qualquer outra iteração, insiro antes de tempProcesso, pois o novo processo deve
						executar antes de tempProcesso */
						else {
							LIS_InserirElementoAntes(_lstRoundRobin, novoProcesso);
							boolInseri = 1;
							/* Parar de percorrer */
							break;
						}
					}
					
					/* Avançar para próximo elemento da lista */
					AvancarCorrenteCircular(_lstRoundRobin);
					LIS_ObterValor(_lstRoundRobin, (void**)&tempProcesso);					
				}	while(tempProcesso != correnteInicial);			
				
				/* Restaurar corrente inicial da lista que havia sido salvo */
				LIS_PopCorrente(_lstRoundRobin);
				
				/* Se ao sair do while, nenhum elemento foi inserido, então o novo processo tem a prioridade mais
				inferior de todas: deve ser inserido antes do corrente da lista para executar por último */
				if (boolInseri == 0) {
					/* Inserir novo processo antes de elemento corrente da lista de round-robin */
					LIS_InserirElementoAntes(_lstRoundRobin, novoProcesso);
					/* Correção: voltar corrente da lista para original */
					AvancarCorrenteCircular(_lstRoundRobin);
				}
				
				DebugMsg("Novo processo prioridade/round-robin foi inserido na lista em sua posicao de prioridade adequada");
				
				/* Se escalonador estava em coma */
				if (_boolComa == 1) {
					long tempoDormir;
					PedirSairComa();
					
					/* Assertiva */
					if (_processoCorrente->politica == REAL_TIME) {
						LevantarErro("Escalonador esta em coma, mas processo corrente eh real time");
					}
					/* Começar a contar o tempo de execucao do processo atual agora, de acordo com sua prioridade */
					tempoDormir = MILISSEGS_RR;
					if (_processoCorrente->politica == PRIORIDADE) {
						tempoDormir *= 9 - _processoCorrente->politica;
					}
					PedirDormir(tempoDormir);
					return;
				}
				/* Se não estava em coma */
				/* Obter novo tamanho da lista de round-robin */
				LIS_ObterTamanhoLista(_lstRoundRobin, &qtdRR);
				/* Se o novo round-robin é o segundo */
				if (qtdRR == 2) {
					/* Trocar contexto para o próprio processo que já está rodando para calcular o tempo até o
					próximo devidamente */
					RegredirCorrenteCircular(_lstRoundRobin);
					PedirAcordar();
				}
				return;
			}
		}
	}
	/* novoProcesso tem política desconhecida */
	else {
		LevantarErro("Erro em politica de novo processo");
	}
}

/*********************************************************************************************
*	Função: organizar troca de contexto
*	Descrição:
*		Função que trata a necessidade imediata de trocar contexto, escolhendo o novo processo que
*		executará e chamando a função que efetivamente trocará o contexto (TrocarContexto).
*********************************************************************************************/
static void OrganizarTrocaContexto() {
	int boolAlgumRR, boolAlgumRT;
	
	/* boolAlgumRR e boolAlgumRT: 1 se houver algum processo Round-Robin/Real-Time */
	boolAlgumRT = VerificarAlgumRT();
	boolAlgumRR = VerificarAlgumRR();
	
	/* Se há algum processo Real-Time */
	if (boolAlgumRT) {
		/* tempo em segundos pra acontecer o próximo processo Real-Time */
		//float tempoProRT;//***
		long milissegundo;
		DEF_tpProcesso* proxRT;
		
		/* milissegundo de 0 a 60 * 1e6 no minuto atual */
		milissegundo = ObterMilissegCorr();
		
		printf("ESC: Segundo do minuto atual: %.4f\n\n", milissegundo / 1e6);
		fflush(stdout);
		
		/* Se o processo atual é Real_Time e se ainda está em seu tempo, terminar de dormir */
		if (_processoCorrente != NULL && _processoCorrente->politica == REAL_TIME) {
			if (((double) milissegundo) / 1e6 > (double) (_processoCorrente->tempoInicio) && (double) (_processoCorrente->tempoInicio + _processoCorrente->tempoDuracao) > ((double) milissegundo) / 1e6) {
				long tempoDormir;
				/* Dormir tempo restante */
				tempoDormir = (long) (_processoCorrente->tempoInicio + _processoCorrente->tempoDuracao);
				tempoDormir = (tempoDormir  * ((long) 1e6)) - milissegundo;
				PedirDormir(tempoDormir);
				return;
			}
		}
		
		/* Se a lista prox estiver vazia, então o próximo a rodar estará na lista past, e os
		processos devem ser colocados da lista past de volta na lista prox. Porém, isso só 
		pode acontecer quando chegar a vez de rodar o próximo processo real-time. */
		
		/* Obter o próximo Real-Time que deve rodar */
		if (0 == LSTP_LstPrioEhVazia(_lstRealTimeProx)) {
			proxRT = LSTP_LstPrioObterPrimeiro(_lstRealTimeProx);
		}
		else {
			proxRT = LSTP_LstPrioObterPrimeiro(_lstRealTimePast);
		}
		
		/* Se está sem seu tempo, rodá-lo */
		if ((double) (milissegundo / 1e6) >= proxRT->tempoInicio && ((double) milissegundo / 1e6) < (double) (proxRT->tempoInicio + proxRT->tempoDuracao)) {
			DEF_tpProcesso* novoProcesso;
			long tempoDormir;
			
			/* Se a lista prox está vazia, então o próximo processo a rodar está na lista past. Agora já
			se pode colocar os processos de volta na lista prox */
			if (1 == LSTP_LstPrioEhVazia(_lstRealTimeProx)) {
				RefrescarListasRealTime();
			}
			
			/* Pegar novamente o próximo processo, desta vez da lista prox */
			novoProcesso = LSTP_LstPrioPop(_lstRealTimeProx);
		
			/* Assertiva */
			if (novoProcesso != proxRT) {
				LevantarErro("Inconsistencia: pop em _lstRealTimeProx eh diferente de ObterPrimeiro em uma das listas");
			}
			
			/* Insere em lista de processos Real-Time que já passaram */
			LSTP_LstPrioInserir(_lstRealTimePast, novoProcesso);
			
			/* Trocar contexto */
			TrocarContexto(novoProcesso);
			tempoDormir = (long) (novoProcesso->tempoInicio + novoProcesso->tempoDuracao);
			tempoDormir = (tempoDormir  * ((long) 1e6)) - milissegundo;
			PedirSairComa();
			PedirDormir(tempoDormir);
			return;
		}
	}
	/* Se o programa chegou aqui, não está na hora do Real-Time. Ou não existem
	processos real-time. Rodar o Round-Robin */
	if (boolAlgumRR) {
		DEF_tpProcesso* novoProcesso;
		long milissegsNovo;
		
		AvancarCorrenteCircular(_lstRoundRobin);
		
		LIS_ObterValor(_lstRoundRobin, (void**)&novoProcesso);
		
		/* Já se sabe que há pelo menos 1 Round-Robin na lista e há apenas processos do tipo 
		Round-Robin. Se só houver 1, o escalonador não precisa acordar de tempos em tempos. */
		
		/* Trocar contexto */
		TrocarContexto(novoProcesso);
		
		/* Se o novoProcesso tem politica somente Round-Robin, tem tempo de execução
			MILISSEGS_RR */
		milissegsNovo = MILISSEGS_RR;
		/* Se o novoProcesso tem politica Prioridade, a cada prioridade superior (até 1)
		acrescenta-se mais MILISSEGS_RR de tempo de execução */
		if (novoProcesso->politica == PRIORIDADE) {
			milissegsNovo *= 9 - novoProcesso->prioridade;
		}
		
		/* Se existir algum Real-Time para rodar futuramente */
		if (boolAlgumRT) {
			DEF_tpProcesso* proxRT;
			int qtdRR;
			long milissegundo, milissegsProxRT;
			
			/* Obter próximo Real-Time (se a lista prox estiver vazia, então próximo Real-Time está na past) */
			if (0 == LSTP_LstPrioEhVazia(_lstRealTimeProx)) {
				proxRT = LSTP_LstPrioObterPrimeiro(_lstRealTimeProx);
			}
			else {
				proxRT = LSTP_LstPrioObterPrimeiro(_lstRealTimePast);
			}
			
			milissegundo = ObterMilissegCorr();
			
			/* Calcular milissegundos pro próximo Real-Time */
			milissegsProxRT = (long) proxRT->tempoInicio * (long) 1e6 - milissegundo;
			if ((double) proxRT->tempoInicio < ((double) milissegundo / 1e6)) {
				milissegsProxRT += (long) 60e6;
			}
			
			/* Obter quantidade de processos RR */
			LIS_ObterTamanhoLista(_lstRoundRobin, &qtdRR);
			
			/* Dormir até o que acontecer primeiro: o próximo round-robin (se houver mais de um) ou o próximo real-time */
			if (qtdRR != 1 && milissegsNovo <= milissegsProxRT) {
				PedirDormir(milissegsNovo);
			}
			else {
				PedirDormir(milissegsProxRT);
			}
		}
		/* Não há processos RT, apenas RR, já trocou contexto */
		else {
			int qtdRR;
			LIS_ObterTamanhoLista(_lstRoundRobin, &qtdRR);
			if (qtdRR == 1) {
				PedirEntrarComa();
				return;
			}
			PedirSairComa();
			PedirDormir(milissegsNovo);
		}
		return;
	}
	/* Não existem processos RR, não está na hora de nenhum RT */
	else {
		/* Pausar um possível RT em execução, não continuar com nenhum outro processo */
		TrocarContexto(NULL);
		
		/* Se existir algum Real-Time para rodar futuramente */
		if (boolAlgumRT) {
			DEF_tpProcesso* proxRT;
			long milissegundo, milissegsProxRT;
			
			/* Obter próximo Real-Time (se a lista prox estiver vazia, então próximo Real-Time está na past) */
			if (0 == LSTP_LstPrioEhVazia(_lstRealTimeProx)) {
				proxRT = LSTP_LstPrioObterPrimeiro(_lstRealTimeProx);
			}
			else {
				proxRT = LSTP_LstPrioObterPrimeiro(_lstRealTimePast);
			}
			
			milissegundo = ObterMilissegCorr();
			
			/* Calcular milissegundos pro próximo Real-Time */
			milissegsProxRT = (long) proxRT->tempoInicio * (long) 1e6 - milissegundo;
			if ((double) proxRT->tempoInicio < ((double) milissegundo / 1e6)) {
				milissegsProxRT += (long) 60e6;
			}
			
			/* Dormir até a hora do próximo Real-Time */
			PedirDormir(milissegsProxRT);
			PedirSairComa();
			return;
		}
		/* Se não há processos RR nem RT */
		else {
			/* Escalonador deve entrar em coma */
			PedirEntrarComa();
			return;
		}
	}
}

/*********************************************************************************************
*	Função: trocar contexto
*	Descrição:
*		Recebe um novo processo que executará em detrimento do que já estava rodando (processo
*		corrente) e manda os sinais SIGSTOP e SIGCONT para que o novo processo retome execução
*		e o processo que já estava rodando, se existir, pause Ao final, o novo processo é o novo
*		processo corrente. Se o novo processo ainda não havia iniciado execução, ele é executado
*		por execv. Se não for possível executar o novo processo, é simplesmente impressa uma
*		mensagem de falha. O escalonador receberá então um SIGCHILD como se um dos processos
*		tivesse sido encerrado. Esse problema deverá ser tratado então como se o novo processo
*		tivesse de fato executado com sucesso mas logo em seguida sido encerrado. Se a função
*		receber um novoProcesso NULL, então ela apenas pausará o processo corrente e não continuará
*		nem executará nada, mudando o processo corrente para NULL.
*********************************************************************************************/
static void TrocarContexto(DEF_tpProcesso* novoProcesso) {	
	/* Enviar sinal de pausa para processo corrente, se existir */
	if (_processoCorrente != NULL) {
		if (-1 == kill(_processoCorrente->pid, SIGSTOP)) {
			LevantarErro("Falha ao enviar sinal de pausa ao processo atualmente em execucao");
		}
	}
	
	/* Se novoProcesso for NULL, não continuar/executar nada */
	if (novoProcesso == NULL) {
		printf("ESC: Processo atual foi pausado\n\n");
		fflush(stdout);
		_processoCorrente = NULL;
		return;
	}
	
	/* Se o novo processo ainda não havia iniciado execução, executá-lo */
	if (novoProcesso->pid == 0) {
		char* parms[2];
		char* strExe;
		int pid;
		int tamStr = 3 + strlen(novoProcesso->nomeArquivo);
		
		strExe = (char*) malloc(tamStr * sizeof(char));
		if (strExe == NULL) {
			LevantarErro("Erro em malloc [2]");
		}
		strExe[0] = '\0';
		strcat(strExe, "./");
		strcat(strExe, novoProcesso->nomeArquivo);
		parms[0] = strExe;
		parms[1] = NULL;
		
		if ((pid = fork()) < 0) {
			LevantarErro("Impossivel executar novo processo");
		}
		else if (pid == 0) {
			if (-1 == execv(novoProcesso->nomeArquivo, parms)) {
				printf("ESC: O programa de nome \"%s\" nao pode ser executado. (Experimente verificar se ele esta na mesma pasta do programa escalonador)\n\n", novoProcesso->nomeArquivo);
				fflush(stdout);
				exit(1);
			}
		}
		novoProcesso->pid = pid;
		
		printf("ESC: Pedido de execucao de novo processo de pid: %d\n\n", novoProcesso->pid);
		fflush(stdout);
	}
	/* Se o novo processo estava pausado, continuá-lo */
	else {
		if (-1 == kill(novoProcesso->pid, SIGCONT)) {
			LevantarErro("Falha ao enviar sinal de continuacao ao processo pausado");
		}
		printf("ESC: Realizada troca de contexto para pid: %d\n\n", novoProcesso->pid);
		fflush(stdout);
		if (_DEBUG == 1) {
			if (novoProcesso->politica == ROUND_ROBIN) {
				printf("ESC DEBUG: Prioridade do processo: 8\n\n");
			}
			else if (novoProcesso->politica == PRIORIDADE) {
				printf("ESC DEBUG: Prioridade do processo: %d\n\n", novoProcesso->prioridade);
			}
			else if (novoProcesso->politica == REAL_TIME) {
				printf("ESC DEBUG: Processo Real-Time\n\n");
			}
			fflush(stdout);
		}
	}
	_processoCorrente = novoProcesso;
}

/*********************************************************************************************
*	Função: tratar processo encerrado
*	Descrição:
*		Função que trata quando um dos processos controlados pelo escalonador termina. Descobre
*		qual dos processos encerrou e depois o retira do banco de dados do escalonador. Se o
*		processo encerrado for o corrente (atualmente em execução), troca o contexto se houverem
*		mais processos restantes.
*********************************************************************************************/
static void TratarProcessoEncerrado() {
	int boolAlgumRR;
	int boolAlgumRT;
	int	pidEncerrado = waitpid(-1, NULL, WNOHANG);
	/* Caso que um filho recebeu um STOP/CONT */
	if (pidEncerrado == 0) {
		/* Não fazer nada */
		return;
	}
	/* Caso de erro */
	if (pidEncerrado == -1) {
		LevantarErro("Erro em percepcao de mudanca de estado de processo filho do escalonador");
	}
	
	boolAlgumRR = VerificarAlgumRR();
	boolAlgumRT = VerificarAlgumRT();
	
	/* Se não existir nenhum processo de nenhum dos tipos, erro */
	if (boolAlgumRR == 0 && boolAlgumRT == 0) {
		LevantarErro("Inconsistencia: tratamento de processo encerrado chamado, mas nao ha processos gerenciados pelo escalonador");
	}
	
	/* Procurar pid em processos Round-Robin, se existir algum */
	if (boolAlgumRR) {
		int achou;
		
		/* Salvar elemento corrente da lista inicial */
		LIS_PushCorrente(_lstRoundRobin);
		
		/* Procurar pid */
		achou = LIS_ProcurarValor(_lstRoundRobin, (void*)&pidEncerrado, CompararPid);
		
		/* Se o processo encerrado estava na lista de round-robin */
		if (achou == 1) {
			DEF_tpProcesso* processoEncerrado;
			/* Obter processo que foi encerrado */
			LIS_ObterValor(_lstRoundRobin, (void**)&processoEncerrado);
			
			/* Se o processo encerrado era o processo corrente */
			if (processoEncerrado == _processoCorrente) {
				int corrige = 0;
				DEF_tpProcesso* corrente;
				
				/* Voltar para corrente da lista salvo */
				LIS_PopCorrente(_lstRoundRobin);
				
				/* Apenas para assertar */
				LIS_ObterValor(_lstRoundRobin, (void**)&corrente);
				if (corrente != _processoCorrente) {
					LevantarErro("Inconsistencia: Elemento corrente da lista de round-robin nao eh processo atualmente em execucao");
				}	/* if */
				
				/* Se o corrente estiver no início, após a exclusão ele vai para a direita.
				Como precisa ir para a esquerda, uma correção precisará ser aplicada */
				if (LIS_CorrenteEstaInicio(_lstRoundRobin)) {
					corrige = 1;
				}	/* if */
				
				/* Excluir elemento e corrigir o corrente, se for necessário e se a lista
				não tiver ficado vazia */
				LIS_ExcluirElemento(_lstRoundRobin);
				if (1 == VerificarAlgumRR()) {		
					if (corrige == 1) {
						RegredirCorrenteCircular(_lstRoundRobin);
					}	/* if */
				}	/* if */
				/* Processo corrente acabou de ser fechado */
				_processoCorrente = NULL;
				/* Mudar contexto para um novo processo */
				PedirAcordar();
				return;
			}	/* if */
			/* Se o processo encerrado não era o processo corrente */
			else {
				/* Excluir processo encerrado da lista */
				LIS_ExcluirElemento(_lstRoundRobin);
				/* Voltar para corrente da lista salvo */
				LIS_PopCorrente(_lstRoundRobin);
				return;
			}	/* else */
		}	/* if */
	}	/* if */
	
	/* Se o processo encerrado não estava na lista round-robin, então ele está na lista de real-time */
	if (boolAlgumRT) {
		int achou;
		DEF_tpProcesso* processoEncerrado;
		
		achou = LSTP_LstPrioBuscar(_lstRealTimePast, &pidEncerrado, CompararPid, (void**)&processoEncerrado);
		if (achou != 1) {
			achou = LSTP_LstPrioBuscar(_lstRealTimeProx, &pidEncerrado, CompararPid, (void**)&processoEncerrado);
		}
		if (achou == 1) {
			/* Se o processo encerrado era o processo corrente */
			if (_processoCorrente == processoEncerrado) {
				/* Processo corrente acabou de ser fechado */
				_processoCorrente = NULL;
				/* Mudar contexto para um novo processo */
				PedirAcordar();
				return;
			}
			/* Se o processo encerrado não era o processo corrente */
			else {
				/* Não precisa fazer nada porque a função buscar já tira o elemento encontrado da lista */
				return;
			}
		}	/* if */
	}	/* if */
	
	LevantarErro("O processo encerrado nao foi encontrado entre os processos gereciados pelo escalonador");
}

/*********************************************************************************************
*	Função: levantar erro
*	Descrição:
*		Escreve a mensagem de erro passada como parâmetro em STDERR e encerra a execução do
*		programa, retornando 1.
*********************************************************************************************/
static void LevantarErro(const char* msg) {
	fprintf(stderr, "@ERRO@ ESC: %s\n", msg);
	exit(1);
}

/*********************************************************************************************
*	Função: mensagem de debug
*	Descrição:
*		Escreve a mensagem de debug passada como parametro em STDOUT se a constante _DEBUG for 1
*		Só deve ser usada se a mensagem nao conter valor de variáveis etc
*********************************************************************************************/
static void DebugMsg(const char* msg) {
	if (_DEBUG == 1) {
		printf("ESC DEBUG: %s\n\n", msg);
		fflush(stdout);
	}
}

/*********************************************************************************************
*	Função: obter milissegundo corrente
*	Descrição:
*		Retorna um inteiro 0 <= retorno < 60 * 1e6 que representa o milissegundo atual dentro do
*		último minuto. Se a constante _DEBUG estiver definida como 1, então o programa começará
*		no segundo DEBUG_TEMPOINICIAL do minuto.
*********************************************************************************************/
static long ObterMilissegCorr() {
	struct timespec tempo;
	long milissegundo;
	int boolMenos1 = 0;
	if (-1 == clock_gettime(CLOCK_REALTIME, &tempo)) {
		LevantarErro("Falha ao obter segundo atual");
	}
	if (_DEBUG == 1) {
		tempo.tv_sec -= _tempoInicio.tv_sec;
		if (tempo.tv_nsec > _tempoInicio.tv_nsec) {
			tempo.tv_nsec -= _tempoInicio.tv_nsec;
		}
		else {
			boolMenos1 = 1;
			tempo.tv_nsec = tempo.tv_nsec + ((long) 1e9 - _tempoInicio.tv_nsec);
		}
	}
	milissegundo = (long) tempo.tv_sec % 60;
	milissegundo *= (long) 1e6;
	milissegundo += (long) (tempo.tv_nsec / 1e3);
	if (_DEBUG == 1) {
		milissegundo += (long) DEBUG_TEMPOINICIAL * 1e6;
		if (boolMenos1 == 1) {
			milissegundo -= (long) 1e6;
		}
		milissegundo %= (long) 60e6;
	}
	return milissegundo;
}

/*********************************************************************************************
*	Função: configurar escalonador
*	Descrição:
*		Define o tratamento de todos os sinais reconhecidos pelo escalonador que devem ser tratados
*********************************************************************************************/
static void ConfigurarEscalonador() {
	
	if (SIG_ERR == signal(SIGUSR1, TratarSinal)) {
		LevantarErro("Sinal SIGUSR1 nao foi configurado com sucesso");
	}
	if (SIG_ERR == signal(SIGCHLD, TratarSinal)) {
		LevantarErro("Sinal SIGCHLD nao foi configurado com sucesso");
	}
}

/*********************************************************************************************
*	Função: tratar sinal
*	Descrição:
*		Trata um sinal recebido
*********************************************************************************************/
static void TratarSinal(int sig) {
	switch(sig) {
		case SIGUSR1:
			MascararSinais();
				HandlerNovoPid();
			DesmascararSinais();
			break;
		case SIGCHLD:
			MascararSinais();
				TratarProcessoEncerrado();
			DesmascararSinais();
			break;
		default:
			LevantarErro("Nao se sabe tratar o sinal recebido");
	}
}

/*********************************************************************************************
*	Função: mascarar sinais
*	Descrição:
*		Bloqueia todos os sinais recebidos pelo programa cujo tratamento é definido pelo usuário,
*		mantendo-os em espera para tratamento posterior
*********************************************************************************************/
static void MascararSinais() {
	sigset_t conjuntoSinais;
	/* Inicializar conjunto de sinais como vazio */
	if (-1 == sigemptyset(&conjuntoSinais)) {
		LevantarErro("Erro ao criar conjunto vazio de sinais");
	}
	
	/* Adicionar sinais tratados pelo escalonador ao conjunto */
	if (-1 == sigaddset(&conjuntoSinais, SIGUSR1)) {
		LevantarErro("Erro ao adicionar sinal SIGUSR1 a conjunto de sinais");
	}
	if (-1 == sigaddset(&conjuntoSinais, SIGCHLD)) {
		LevantarErro("Erro ao adicionar sinal SIGCHLD a conjunto de sinais");
	}
	
	/* Bloquear sinais do conjunto */
	if (-1 == sigprocmask(SIG_BLOCK, &conjuntoSinais, NULL)) {
		LevantarErro("Falha ao bloquear sinais");
	}
}

/*********************************************************************************************
*	Função: desmascarar sinais
*	Descrição:
*		Desbloqueia todos os sinais recebidos pelo programa cujo tratamento é definido pelo usuário,
*		retirando-os da espera para tratá-los
*********************************************************************************************/
static void DesmascararSinais() {
	sigset_t conjuntoSinais;
	/* Inicializar conjunto de sinais como vazio */
	if (-1 == sigemptyset(&conjuntoSinais)) {
		LevantarErro("Erro ao criar conjunto vazio de sinais");
	}
	
	/* Adicionar sinais tratados pelo escalonador ao conjunto */
	if (-1 == sigaddset(&conjuntoSinais, SIGUSR1)) {
		LevantarErro("Erro ao adicionar sinal SIGUSR1 a conjunto de sinais");
	}
	if (-1 == sigaddset(&conjuntoSinais, SIGCHLD)) {
		LevantarErro("Erro ao adicionar sinal SIGCHLD a conjunto de sinais");
	}
	
	/* Desbloquear sinais do conjunto */
	if (-1 == sigprocmask(SIG_UNBLOCK, &conjuntoSinais, NULL)) {
		LevantarErro("Falha ao desbloquear sinais");
	}
}

/*********************************************************************************************
*	Função: buscar conflitos lista real-time
*	Descrição:
*		Recebe um novo processo Real-Time e um processo Real-Time já na lista de prioridades e busca
*		um conflito de tempo entre o processo e o processo da lista.
*	Retorno:
*		0	-	Em caso de não haver conflito
*		1	-	Em caso de haver conflito
*********************************************************************************************/
static int BuscarConflitosLstRT(void* novoProcesso, void* processoLst) {
		DEF_tpProcesso* processo = (DEF_tpProcesso*) novoProcesso;
	DEF_tpProcesso* procTemp = (DEF_tpProcesso*) processoLst;
	
	DEF_tpProcesso* menorInicio = processo,* maiorInicio = processo;
	
	if (procTemp->tempoInicio >= processo->tempoInicio) {
		maiorInicio = procTemp;
	}
	else {
		menorInicio = procTemp;
	}
	if (menorInicio->tempoInicio + menorInicio->tempoDuracao > maiorInicio->tempoInicio) {
		return 1;
	}
	return 0;
}

/*********************************************************************************************
*	Função: conferir validade real-time
*	Descrição:
*		Recebe um processo Real-Time e verifica se ele é valido, ou seja, se a soma de seu tempo de
*		inicio e seu tempo de duracao nao ultrapassa 60 e se nao ha conflitos com outros processos
*		preexistentes.
*	Retorno:
*		0		-	Em caso de validade
*		-1	-	Em caso de invalidade. Nesse caso, uma mensagem é exibida ao usuário com o motivo da
*					invalidade.
*********************************************************************************************/
static int ConferirValidadeRT(DEF_tpProcesso* processo) {
	int achouConflito;
	DEF_tpProcesso* processoConflito;
	if (processo->tempoDuracao <= 0) {
		printf("ESC: Programa de arquivo \"%s\" nao pode ser executado porque sua duracao eh invalida.\n\n", processo->nomeArquivo);
		fflush(stdout);
		return -1;
	}
	if (processo->tempoInicio > 60 || processo->tempoInicio < 0) {
		printf("ESC: Programa de arquivo \"%s\" nao pode ser executado porque seu tempo de inicio eh invalido.\n\n", processo->nomeArquivo);
		fflush(stdout);
		return -1;
	}
	if (processo->tempoInicio + processo->tempoDuracao > 60) {
		printf("ESC: Programa de arquivo \"%s\" nao pode ser executado porque seu inicio + duracao ultrapassam 60 segundos.\n\n", processo->nomeArquivo);
		fflush(stdout);
		return -1;
	}
	/* Busca um conflito de tempo do novo processo com cada processo da lista de Real-Time */
	achouConflito = LSTP_LstPrioBuscar(_lstRealTimePast, processo, BuscarConflitosLstRT, (void**)&processoConflito);
	if (achouConflito == 1) {
		printf("ESC: Programa de arquivo \"%s\" nao pode ser executado porque ha um conflito com o processo de arquivo \"%s\".\n\n", processo->nomeArquivo, processoConflito->nomeArquivo);
		fflush(stdout);
		return -1;
	}
	achouConflito = LSTP_LstPrioBuscar(_lstRealTimeProx, processo, BuscarConflitosLstRT, (void**)&processoConflito);
	if (achouConflito == 1) {
		printf("ESC: Programa de arquivo \"%s\" nao pode ser executado porque ha um conflito com o processo de arquivo \"%s\".\n\n", processo->nomeArquivo, processoConflito->nomeArquivo);
		fflush(stdout);
		return -1;
	}
	return 0;
}

/*********************************************************************************************
*	Função: verificar algum round-robin
*	Descrição:
*		Verifica se há algum processo na lista _lstRoundRobin.
*	Retorno:
*		1	-	Há pelo menos um processo round-robin na lista.
*		0	-	Não há processos. A lista está vazia.
*********************************************************************************************/
static int VerificarAlgumRR() {
	int qtdRR;
	LIS_ObterTamanhoLista(_lstRoundRobin, &qtdRR);
	if (qtdRR > 0) {
		return 1;
	}
	else {
		return 0;
	}
}

/*********************************************************************************************
*	Função: verificar algum real-time
*	Descrição:
*		Verifica se há algum processo Real-time em uma das listas: _lstRealTimePast e _lstRealTimeProx
*	Retorno:
*		1	-	Há pelo menos um processo real-time em uma das listas.
*		0	-	Não há processos. Ambas as listas estão vazias.
*********************************************************************************************/
static int VerificarAlgumRT() {
	if (0 == LSTP_LstPrioEhVazia(_lstRealTimeProx) || 0 == LSTP_LstPrioEhVazia(_lstRealTimePast)) {
		return 1;
	}
	else {
		return 0;
	}
}

/*********************************************************************************************
*	Função: avancar corrente circular
*	Descrição:
*		Recebe uma lista e avança seu elemento corrente. Se o elemento corrente já era o último
*		elemento, ele volta para o início.
*********************************************************************************************/
static void AvancarCorrenteCircular(LIS_tppLista lst) {
	if (1 == LIS_CorrenteEstaFinal(lst)) {
		LIS_IrInicioLista(lst);
	}
	else {
		LIS_AvancarElementoCorrente(lst, 1);
	}
}

static void RegredirCorrenteCircular(LIS_tppLista lst) {
	if (1 == LIS_CorrenteEstaInicio(lst)) {
		LIS_IrFinalLista(lst);
	}
	else {
		LIS_AvancarElementoCorrente(lst, -1);
	}
}

/*********************************************************************************************
*	Função: obter tempo
*	Descrição:
*		Retorna o tempo atual no tipo struct timespec
*********************************************************************************************/
static struct timespec ObterTempo() {
	struct timespec tempo;
	if (-1 == clock_gettime(CLOCK_REALTIME, &tempo)) {
		LevantarErro("Falha ao obter tempo");
	}
	return tempo;
}

/*********************************************************************************************
*	Função: obter tempo passado
*	Descrição:
*		Recebe um tempo inicial em struct timespec e calcula o tempo atual passado até então
*	Retorno:
*		Retorna o tempo passado em microssegundos
*********************************************************************************************/
static long ObterTempoPassado(struct timespec inicial) {
	struct timespec tempo;
	long diffSec;
	long diffNanoSec;
	long diffTotalMicro;
	if (-1 == clock_gettime(CLOCK_REALTIME, &tempo)) {
		LevantarErro("Falha ao obter tempo decorrido");
	}
	diffSec = (long) (tempo.tv_sec - inicial.tv_sec);
	diffNanoSec = tempo.tv_nsec - inicial.tv_nsec;
	diffTotalMicro = round(diffSec * 1.0e6) + round(diffNanoSec / 1.0e3);
	return diffTotalMicro;
}

/*********************************************************************************************
*	Função: refrescar listas real-time
*	Descrição:
*		Deve ser chamada com pelo menos uma das listas Real-Time não vazia (retorno de
*		VerificarAlgumRT deve ser 1). Caso a lista _lstRealTimeProx esteja vazia, esta função passa
*		todos os processos da _lstRealTimePast para a _lstRealTimeProx
*********************************************************************************************/
static void RefrescarListasRealTime() {
	if (1 == LSTP_LstPrioEhVazia(_lstRealTimeProx)) {
		int passeiUm = 0;
		/* Passar elementos para a lista de próximos até a lista de anteriores ficar vazia */
		while(0 == LSTP_LstPrioEhVazia(_lstRealTimePast)) {
			DEF_tpProcesso* tempProcesso;
			tempProcesso = LSTP_LstPrioPop(_lstRealTimePast);
			LSTP_LstPrioInserir(_lstRealTimeProx, tempProcesso);
			passeiUm = 1;
		}
		/* Assertiva */
		if (passeiUm == 0) {
			LevantarErro("Nao foi possivel passar os processos Real-Time de uma lista para outra");
		}
	}
}

/*********************************************************************************************
*	Função: pedir pra entrar em coma
*	Descrição:
*		Após ser chamada, essa função fará o escalonador entrar em coma no seu próximo main loop
*********************************************************************************************/
static void PedirEntrarComa() {
	if (_boolComa == 0) {
		printf("ESC: Escalonador entrando em coma\n\n");
		fflush(stdout);
	}
	_boolComa = 1;
}

/*********************************************************************************************
*	Função: pedir pra sair do coma
*	Descrição:
*		Após ser chamada, essa função fará o escalonador sair do coma no seu próximo main loop
*********************************************************************************************/
static void PedirSairComa() {
	if (_boolComa == 1) {
		printf("ESC: Escalonador saindo de coma\n\n");
		fflush(stdout);
	}
	_boolComa = 0;
}

/*********************************************************************************************
*	Função: pedir pra dormir
*	Descrição:
*		Após ser chamada, essa função atribuirá o tempo passado como o tempo que o escalonador
*		irá dormir no seu próximo main loop
*********************************************************************************************/
static void PedirDormir(long tempo) {
	_tempoSono = tempo;
}

/*********************************************************************************************
*	Função: pedir pra acordar
*	Descrição:
*		Após ser chamada, essa função fará o escalonador não dormir no seu próximo main loop
*********************************************************************************************/
static void PedirAcordar() {
	_tempoSono = 0;
}

/*********************************************************************************************
*	Função: imprimir processo
*	Descrição:
*		Recebe um ponteiro para processo e imprime dados sobre ele (com excessão do pid). Pode
*		receber também uma mensagem que será impressa antes dos dados do processo (ou NULL), para
*		não imprimir nada antes.
*********************************************************************************************/
static void ImprimirProcesso(DEF_tpProcesso* p, const char* msg) {
	if (msg != NULL) {
		printf("ESC: %s:\n", msg);
	}
	printf("ESC: Nome do arquivo do processo:\t%s\n", p->nomeArquivo);
	
	printf("ESC: Politica do processo:\t\t");
	switch(p->politica) {
		case ROUND_ROBIN:
			printf("Round-Robin");
			break;
		case PRIORIDADE:
			printf("Prioridade");
			break;
		case REAL_TIME:
			printf("Real-Time");
			break;
		default:
			LevantarErro("Nome da politica desconhecido");
	}
	puts("");
	if (p->politica == PRIORIDADE) {
		printf("ESC: Prioridade do processo:\t\t%d\n", p->prioridade);
	}
	else if (p->politica == REAL_TIME) {
		printf("ESC: Tempo de inicio do processo:\t%d\n", p->tempoInicio);
		printf("ESC: Tempo de duracao do processo:\t%d\n", p->tempoDuracao);
	}
	puts("");
	fflush(stdout);
}
