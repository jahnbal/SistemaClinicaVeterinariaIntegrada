/*
 * venda.h — Módulo de Vendas durante o Atendimento Veterinário
 *
 * Integra a lógica de venda da LojaIntegrada ao fluxo de atendimento
 * da clínica veterinária.
 *
 * Conceitos de LP2 demonstrados neste módulo:
 *   [1] Funções recursivas   → CalculaTotalRecursivo()
 *   [2] Duplos ponteiros     → RemoveItemCarrinhoById(), LimpaCarrinho()
 *   [3] Alocação dinâmica    → malloc() para nós do carrinho e relatório
 *   [4] Vetores dinâmicos    → realloc() na geração de relatório
 *   [5] Matrizes dinâmicas   → matriz alocada em GerarRelatorioVendas()
 *   [6] Listas encadeadas    → struct ItemCarrinho com ponteiro *proximo
 *   [7] Ponteiros de função  → FuncExibeProduto em ListaProdutosVenda()
 */

#ifndef VENDA_H
#define VENDA_H

#include <stdio.h>
#include <stdlib.h>
#include "produtos.h"
#include "pets.h"

/* ═══════════════════════════════════════════════════════════════════════════
 *  ESTRUTURAS
 * ═══════════════════════════════════════════════════════════════════════════ */

/*
 * [LP2 — Lista Encadeada]
 * Cada nó representa um item adicionado ao carrinho.
 * O campo *proximo forma a lista encadeada simplesmente ligada.
 */
typedef struct ItemCarrinho {
    int    id_produto;
    char   nome_produto[TAM_NOME_PRODUTO];
    float  preco_unitario;
    int    quantidade;
    float  total_item;
    struct ItemCarrinho *proximo; /* próximo nó da lista encadeada */
} ItemCarrinho;

/*
 * Carrinho de compras: cabeça da lista encadeada + metadados.
 */
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

/* ═══════════════════════════════════════════════════════════════════════════
 *  [LP2 — Ponteiro de Função]
 *  Tipo para funções de exibição de produto. Permite trocar o formato
 *  de listagem sem alterar ListaProdutosVenda().
 * ═══════════════════════════════════════════════════════════════════════════ */
typedef void (*FuncExibeProduto)(Produto *);

/* ═══════════════════════════════════════════════════════════════════════════
 *  OPERAÇÕES COM CARRINHO
 * ═══════════════════════════════════════════════════════════════════════════ */

/* Inicializa um carrinho vazio */
void InicializaCarrinho(Carrinho *carrinho);

/*
 * Adiciona (ou atualiza) um item no carrinho.
 * Aloca um novo nó dinamicamente. Verifica estoque disponível.
 * Retorna 1 em sucesso, 0 em falha.
 */
int AdicionaItemCarrinho(Carrinho *carrinho,
                         int id_produto,
                         int quantidade,
                         FILE *arq_produtos);

/*
 * [LP2 — Duplo Ponteiro]
 * Remove do carrinho o item com id_produto informado.
 * Usa Carrinho** para poder atualizar a cabeça da lista quando necessário.
 * Retorna 1 em sucesso, 0 se não encontrado.
 */
int RemoveItemCarrinhoById(Carrinho **carrinho, int id_produto);

/*
 * Libera toda a memória da lista encadeada e zera o carrinho.
 * (O duplo ponteiro é demonstrado em RemoveItemCarrinhoById acima.)
 */
void LimpaCarrinho(Carrinho *carrinho);

/* ═══════════════════════════════════════════════════════════════════════════
 *  CÁLCULO E EXIBIÇÃO
 * ═══════════════════════════════════════════════════════════════════════════ */

/*
 * [LP2 — Função Recursiva]
 * Percorre a lista recursivamente somando total_item de cada nó.
 */
float CalculaTotalRecursivo(ItemCarrinho *item);

/*
 * Lista produtos ativos do arquivo usando a função de exibição recebida
 * por ponteiro. [LP2 — Ponteiro de Função]
 */
void ListaProdutosVenda(FILE *arq_produtos, FuncExibeProduto exibe);

/* Implementação padrão de FuncExibeProduto */
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
