#include <stdio.h>
#include <stdlib.h>
#include "utilidades.h"



int getOpcao(void) {
    int opcao;
    int c;

    scanf("%i", &opcao);

//  Limpa o buffer
    while ((c = getchar()) != '\n' && c != EOF);

    return opcao;
}

void limparTela(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}