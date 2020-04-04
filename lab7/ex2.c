#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <wait.h>
#define FIFO "minhafifo"
#define OPENMODE (O_RDONLY | O_NONBLOCK)

int main()
{
	int fifo,pid,pid2;
	char mensagem1[]= "Melancia";
	char mensagem2[]=" sem caroco\n";
	if(access(FIFO,F_OK)==-1)
	{
		if(mkfifo(FIFO,S_IRUSR | S_IWUSR) !=0)
		{
			fprintf(stderr,"Erro ao criar FIFO %s\n",FIFO);
			return -1;
		}
	}
	if((pid=fork())<0)
	{
		fprintf(stderr,"Erro ao criar filho 1\n");
		return -1;
	}
	if (pid == 0) // filho 1
	{
		if((fifo=open(FIFO,O_WRONLY))<0)
		{
			fprintf(stderr,"Erro ao abrir fifo %s\n",FIFO);
			return -1;
		}
		write(fifo,mensagem1,strlen(mensagem1));
		close(fifo);		
	}
	else // pai
	{
		if((pid2=fork())<0)
		{
			fprintf(stderr,"Erro ao criar filho 2\n");
			return -1;
		}
		if(pid2 == 0) // filho 2
		{
			if((fifo=open(FIFO,O_WRONLY))<0)
			{
				fprintf(stderr,"Erro ao abrir fifo %s\n",FIFO);
				return -1;
			}
			sleep(1); // para o filho 2 imprimir depois e a mensagem ficar bonita
			write(fifo,mensagem2,strlen(mensagem2));
			close(fifo);
		}
		else // pai
		{
			char ch;
			if((fifo=open(FIFO,OPENMODE))<0)
			{
				fprintf(stderr,"Erro ao abrir fifo %s\n",FIFO);
				return -1;
			}
			wait(NULL);
			wait(NULL);
			while(read(fifo, &ch, sizeof(ch)) > 0) {
				putchar(ch);
			}
		}
	} // pai
	close(fifo);
	return 0;
}
