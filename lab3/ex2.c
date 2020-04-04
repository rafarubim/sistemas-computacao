#include<stdio.h>
#include<signal.h>
#include<stdlib.h>

#define EVER ;;

/*
	"O que é imortal, não morre no final" - Sandy & Junior
*/
int main()
{
	signal(SIGKILL, SIG_IGN);
	raise(SIGKILL);
	for(EVER);
	return 0;
}
	
