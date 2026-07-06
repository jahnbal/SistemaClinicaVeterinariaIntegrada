#include "produtos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int VerificaProduto(Produto *produto, FILE *arq) {
  Produto temp;

  rewind(arq);
  while (fread(&temp, sizeof(Produto), 1, arq) == 1) {
    if (temp.id == produto->id) {

      printf("Produto encontrado!\n");
      printf("\n%.2fR$ , %d unidades\n", temp.preco, temp.quantidade);
      return 1;
    }
  }
  return 0;
}

int InsereProduto(Produto *produto, FILE *arq) {
  if (VerificaProduto(produto, arq)) {
    printf("Produto com ID %d ja existe!\n", produto->id);
    return 0;
  }

  fseek(arq, 0, SEEK_END);
  fwrite(produto, sizeof(Produto), 1, arq);
  fflush(arq);

  return 1;
}

/*

  Copia todos os registros, exceto o que sera removido,
   para um arquivo temporario; em seguida apaga o arquivo original e
   renomeia o temporario para o nome original.

   IMPORTANTE: como o arquivo original precisa ser fechado, apagado e
   recriado, o FILE* recebido em 'arq' fica invalido apos a chamada.
   O chamador deve reabrir o arquivo (fopen) logo depois de usar esta
   funcao, antes de fazer qualquer outra operacao com ele.

   Retorna 1 se removeu com sucesso, 0 caso o produto nao seja
   encontrado ou ocorra algum erro de arquivo. */

int RemoveProduto(Produto *produto, FILE *arq) {
  Produto atual;
  int encontrado = 0;

  if (!VerificaProduto(produto, arq)) {
    printf("Produto com ID %d nao encontrado!\n", produto->id);
    return 0;
  }

  FILE *temp = fopen(ARQUIVO_TEMP_PRODS, "wb");
  if (temp == NULL) {
    perror("Erro ao criar arquivo temporario");
    return 0;
  }

  rewind(arq);
  while (fread(&atual, sizeof(Produto), 1, arq) == 1) {
    if (atual.id == produto->id) {
      encontrado = 1;
      continue;
    }
    fwrite(&atual, sizeof(Produto), 1, temp);
  }

  fclose(temp);
  fclose(arq);

  if (!encontrado) {
    remove(ARQUIVO_TEMP_PRODS);
    return 0;
  }

  if (remove(ARQUIVO_PRODS) != 0) {
    perror("Erro ao remover arquivo original");
    return 0;
  }

  if (rename(ARQUIVO_TEMP_PRODS, ARQUIVO_PRODS) != 0) {
    perror("Erro ao renomear arquivo temporario");
    return 0;
  }

  return 1;
}

// ListaTodosProdutos — imprime tabela de produtos ativos
void ListaTodosProdutos(FILE *arq) {
  Produto p;
  rewind(arq);
  printf("  %-5s | %-30s | %-11s | %s\n", "ID", "NOME", "PRECO", "ESTOQUE");
  printf("  %s\n", "-------------------------------------------------------");
  int total = 0;
  while (fread(&p, sizeof(Produto), 1, arq) == 1) {
    if (p.ativo) {
      printf("  %-5d | %-30s | R$ %-8.2f | %d\n", p.id, p.nome, p.preco,
             p.quantidade);
      total++;
    }
  }
  if (total == 0)
    printf("  (nenhum produto cadastrado)\n");
  printf("\n");
}

//BuscaProdutoPorId — retorna Produto* alocado dinamicamente ou NULL
Produto *BuscaProdutoPorId(int id, FILE *arq) {
  Produto temp;
  rewind(arq);
  while (fread(&temp, sizeof(Produto), 1, arq) == 1) {
    if (temp.id == id && temp.ativo) {
      Produto *encontrado = (Produto *)malloc(sizeof(Produto));
      if (encontrado)
        *encontrado = temp;
      return encontrado;
    }
  }
  return NULL;
}

//DescontaEstoque — reduz a quantidade em estoque de um produto.
// Reescreve apenas o registro alterado diretamente no arquivo.
int DescontaEstoque(int id, int quantidade) {

  FILE* arq = fopen(ARQUIVO_PRODS, "rb+");
  if (!arq) {
    printf("Erro ao abrir arquivo");
    return 0;
  }
  Produto temp;
  rewind(arq);

  while (fread(&temp, sizeof(Produto), 1, arq) == 1) {
    if (temp.id == id && temp.ativo) {
      if (temp.quantidade < quantidade) {

        fclose(arq);
        return 0;
      }
      temp.quantidade -= quantidade;

      long pos_atual = ftell(arq);
      fseek(arq, pos_atual - sizeof(Produto), SEEK_SET);

      fwrite(&temp, sizeof(Produto), 1, arq);
      fflush(arq);

      fclose(arq);
      return 1;
    }
  }
  printf("  Produto ID %d nao encontrado para desconto de estoque.\n", id);
  fclose(arq);
  return 0;
}
