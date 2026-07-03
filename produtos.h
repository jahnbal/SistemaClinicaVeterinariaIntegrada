#ifndef PRODUTOS_H
#define PRODUTOS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ARQUIVO_PRODS "produtos.dat"
#define NOME_ARQUIVO "produtos.dat" // IMPORTANTE VER ISSO DEPOIS
#define ARQUIVO_TEMP_PRODS "produtos_temp.dat"

#define TAM_DESCRICAO_PRODUTO 150
#define TAM_NOME_PRODUTO 30

typedef struct {

  char nome[TAM_NOME_PRODUTO];
  int id;
  float preco;
  int quantidade;
  int ativo; /* 1 eh ativo | 0 eh inativo */
  char descricao[TAM_DESCRICAO_PRODUTO];

} Produto;

int VerificaProduto(Produto *produto, FILE *arq);
int InsereProduto(Produto *produto, FILE *arq);
int RemoveProduto(Produto *produto, FILE *arq);

// R
void ListaTodosProdutos(FILE *arq);

/*
 * Busca um produto pelo ID.
 * Retorna um ponteiro para Produto alocado dinamicamente (uso: alocação
 * dinâmica – req. LP2). O chamador é responsável por liberar a memória
 * com free(). Retorna NULL se não encontrado ou inativo.
 */
Produto *BuscaProdutoPorId(int id, FILE *arq);

/*
 * Desconta 'quantidade' unidades do estoque do produto com o ID informado.
 * Retorna 1 em sucesso, 0 em falha (estoque insuficiente ou produto não encontrado).
 */
int DescontaEstoque(int id, int quantidade, FILE *arq);
#endif
