#include "vendas.h"
#include "produtos.h"
#include <stdio.h>

#define ARQUIVO_VENDAS "vendas.txt"

int RealizarVenda(Produto produto) {

  int res;

  FILE *arq = fopen(ARQUIVO_PRODS, "rb");

  if (arq) {

    res = VerificaProduto(&produto, arq);

    if (res == 1) {
      printf("Produto encontrado!");

      RemoveProduto(&produto, arq);

      printf("Venda Realizada!\n");
      printf("Valor: %f\n", produto.preco);

      RegistrarVenda(produto);

      return 1;
    } else {

      printf("produto nao encontrado\n");
      return 0;
    }

  } else {

    printf("Nao foi possivel abrir arquivo de produtos\n");
    return -1;
  }
}

void RegistrarVenda(Produto produto) {

  FILE *arq = fopen(ARQUIVO_VENDAS, "w");

  fprintf(arq, " %s(%d) : %fR$\n", produto.nome, produto.id, produto.preco);

  fclose(arq);
}

int BuscarVenda(Produto produto, FILE *arq) {

  int busca;
  arq = fopen(ARQUIVO_PRODS, "r");

  if (!arq) {
    printf("Erro ao abrir o arquivo de vendas\n");
    return -1;
  }

  fseek(arq, 30, SEEK_SET);

  while (fscanf(arq, "%d", &busca) != EOF) {

    if (produto.id == busca) {
      printf("Esse produto foi vendido!");
      return 1;
    }
  }

  printf("O produto nao foi vendido!");
  return 0;
}
