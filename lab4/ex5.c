#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void erro(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}

void capturei(int sinal) {
	erro("Divisao por 0");
}

int main() {
	int a, b;
	if (-1 == (long)signal(SIGFPE, capturei)) {
		erro("Instalacao de tratador inadequada");
	}
	printf("Digite dois numeros: ");
	scanf(" %d %d", &a, &b);
	printf("\na + b = %d\n", a + b);
	printf("a - b = %d\n", a - b);
	printf("a * b = %d\n", a * b);
	printf("a / b = %d\n", a / b);
	return 0;
}
