#include "utilidades.h"
#include <stdio.h>
#include <stdlib.h>

int getOpcao(void) {
  int opcao;
  int c;

  scanf("%i", &opcao);

  //  Limpa o buffer
  while ((c = getchar()) != '\n' && c != EOF)
    ;

  return opcao;
}

int ContaDigitos(int numero) {

  int contador = 0;

  if (numero == 0) {
    return 0;
  }

  if (numero < 0) {
    numero = -numero;
  }

  while (numero > 0) {

    numero /= 10;

    contador++;
  }

  return contador;
}
