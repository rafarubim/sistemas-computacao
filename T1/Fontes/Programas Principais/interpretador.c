#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include"definicoes.h"
#include<wait.h>

int main (void) 
{
	int  c,id;
	FILE *myfp;
	DEF_tpProcesso proc;
	char num_pipe[11];
	int fd[2];
	if(pipe(fd) < 0) 
	{ 
		puts ("Erro ao abrir os pipes"); 
		exit (-1); 
	}
	sprintf(num_pipe, "%d", fd[0]);
    if((id=fork())<0)
	{
		printf("Erro na criação do processo");
		exit(1);
	}
	if(id!=0)
	{
		int teste;
		if ((myfp = fopen ("exec.txt", "r")) == NULL) 
		{
    		printf ("nao conseguiu abrir arquivo!");
    		exit(1);
    	}    
		close(fd[0]);//interpretador nao vai ler nada do pipe
		while (fscanf(myfp,"Exec %s",proc.nomeArquivo)==1) 
		{
			sleep(1);
			c = fgetc(myfp);
			switch(c)
			{
				case '\n':
				{
					proc.politica=ROUND_ROBIN;break;
				}
				case ' ':
				{
					c = fgetc(myfp); 
					switch(c)
					{
						case 'P':
						{
							proc.politica=PRIORIDADE;
							fscanf(myfp,"R=%d\n",&(proc.prioridade));
							break;
						}
						case 'I':
						{
							proc.politica=REAL_TIME;
							fscanf(myfp,"=%d D=%d\n",&(proc.tempoInicio),&(proc.tempoDuracao));
							break;
						}
					}
				}
			}
			proc.pid = 0;
			write(fd[1],&proc,sizeof(proc));
			teste=kill(id,SIGUSR1);
			if(teste==-1)
			{
				printf("Falha na comunicação com o escalonador");
				exit(-1);
			}
		}
		
		close(fd[1]);
		fclose(myfp);
	}
 	else
	{
		char* const a[] = {"./escalonador", num_pipe, NULL};
		close(fd[1]); //escalonador nao vai escrever nada no pipe
		execv("escalonador", a);
	}
  
  wait(NULL);
  
  return 0;
}
