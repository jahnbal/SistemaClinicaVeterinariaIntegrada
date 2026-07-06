#ifndef VENDA_H
#define VENDA_H

#include <stdio.h>
#include <stdlib.h>
#include "produtos.h"
#include "pets.h"

typedef void (*FuncExibeProduto)(Produto *produto);

// Cada item do carrinho é um nó da lista encadeada
typedef struct ItemCarrinho {
    int    id_produto;
    char   nome_produto[TAM_NOME_PRODUTO];
    float  preco_unitario;
    int    quantidade;
    float  total_item;
    struct ItemCarrinho *proximo; /* próximo nó da lista encadeada */
} ItemCarrinho;


// Carrinho de compras: cabeça da lista encadeada + metadados.
typedef struct {
    ItemCarrinho *cabeca; /* [LP2 — Lista Encadeada] cabeça da lista */
    int   qtd_itens;
    float total;
} Carrinho;

/*
 * Registro persistente de venda (gravado em vendas.dat).
 * Usa vetores de tamanho fixo para as colunas da nota fiscal.
 */
#define MAX_ITENS_NOTA      50
#define NOME_ARQUIVO_VENDAS "vendas.dat"

typedef struct {
    int   id_venda;
    int   id_pet;
    int   id_cliente;
    int   qtd_itens;
    /* [LP2 — Vetores Dinâmicos] arrays paralelos dos itens vendidos */
    int   ids_produtos[MAX_ITENS_NOTA];
    int   quantidades [MAX_ITENS_NOTA];
    float precos      [MAX_ITENS_NOTA];
    float total;
    float valor_pago;
    float troco;
} RegistroVenda;




// Inicializa um carrinho vazio 
void InicializaCarrinho(Carrinho *carrinho);

//Adiona um item ao carrinho, alocando dinamicamente um novo nó da lista encadeada.
int AdicionaItemCarrinho(Carrinho *carrinho,
                         int id_produto,
                         int quantidade,
                         FILE *arq_produtos);

// Remove um item do carrinho pelo ID do produto, liberando o nó da lista encadeada.
int RemoveItemCarrinhoById(Carrinho **carrinho, int id_produto);

// Libera todos os itens do carrinho
void LimpaCarrinho(Carrinho *carrinho);



// Percorre recursivamente a lista encadeada do carrinho e calcula o total da compra.
float CalculaTotalRecursivo(ItemCarrinho *item);

// Lista todos os produtos disponíveis para venda, chamando a função de callback exibe().
void ListaProdutosVenda(FILE *arq_produtos, FuncExibeProduto exibe);

// Implementação padrão de FuncExibeProduto
void ExibeProdutoPadrao(Produto *p);

/* Exibe o carrinho formatado na tela */
void ExibeCarrinho(Carrinho *carrinho);

/* ═══════════════════════════════════════════════════════════════════════════
 *  FLUXO PRINCIPAL DE VENDA
 * ═══════════════════════════════════════════════════════════════════════════ */

/* Imprime a nota fiscal completa no terminal */
void ImprimirNotaFiscal(Carrinho *carrinho, Pet *pet, float valor_pago);

/* Grava o RegistroVenda no arquivo vendas.dat. Retorna 1 em sucesso */
int SalvaRegistroVenda(RegistroVenda *venda, FILE *arq_vendas);

/*
 * [LP2 — Matriz Dinâmica]
 * Lê todos os registros de vendas e exibe um relatório em formato de
 * tabela usando uma matriz alocada dinamicamente (linhas × colunas).
 * Colunas: [ID venda | ID pet | ID cliente | Qtd itens | Total | Troco]
 */
void GerarRelatorioVendas(FILE *arq_vendas);

#endif /* VENDA_H */
