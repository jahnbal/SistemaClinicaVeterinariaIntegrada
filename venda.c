/*
 * venda.c — Implementação do módulo de vendas durante o atendimento
 *
 * Lógica central portada da LojaIntegrada e adaptada ao contexto da
 * clínica veterinária: o "cliente" é identificado pelo pet em atendimento,
 * o estoque é controlado em arquivo binário e o carrinho é uma lista
 * encadeada alocada dinamicamente.
 */

#include "venda.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ═══════════════════════════════════════════════════════════════════════════
 *  CARRINHO — INICIALIZAÇÃO
 * ═══════════════════════════════════════════════════════════════════════════ */

void InicializaCarrinho(Carrinho *carrinho) {
    carrinho->cabeca    = NULL;
    carrinho->qtd_itens = 0;
    carrinho->total     = 0.0f;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  CARRINHO — ADIÇÃO DE ITEM
 *  [LP2 — Alocação Dinâmica] malloc para cada novo nó da lista
 * ═══════════════════════════════════════════════════════════════════════════ */

int AdicionaItemCarrinho(Carrinho *carrinho,
                         int id_produto,
                         int quantidade,
                         FILE *arq_produtos)
{
    /* Busca o produto no arquivo (retorna Produto* alocado dinamicamente) */
    Produto *prod = BuscaProdutoPorId(id_produto, arq_produtos);
    if (prod == NULL) {
        printf("  ERRO: Produto ID %d nao encontrado ou inativo.\n", id_produto);
        return 0;
    }
    if (prod->quantidade < quantidade) {
        printf("  AVISO: Estoque insuficiente (disponivel: %d unidades).\n",
               prod->quantidade);
        free(prod);
        return 0;
    }

    /* Se o produto já está no carrinho, apenas atualiza a quantidade */
    ItemCarrinho *atual = carrinho->cabeca;
    while (atual != NULL) {
        if (atual->id_produto == id_produto) {
            atual->quantidade  += quantidade;
            atual->total_item   = atual->preco_unitario * atual->quantidade;
            carrinho->total     = CalculaTotalRecursivo(carrinho->cabeca);
            printf("  Quantidade atualizada: %s (qtd total: %d).\n",
                   atual->nome_produto, atual->quantidade);
            free(prod);
            return 1;
        }
        atual = atual->proximo;
    }

    /* Produto novo no carrinho — aloca nó dinamicamente [LP2] */
    ItemCarrinho *novo = (ItemCarrinho *)malloc(sizeof(ItemCarrinho));
    if (novo == NULL) {
        printf("  ERRO: Memoria insuficiente para adicionar item.\n");
        free(prod);
        return 0;
    }

    novo->id_produto     = id_produto;
    strncpy(novo->nome_produto, prod->nome, TAM_NOME_PRODUTO - 1);
    novo->nome_produto[TAM_NOME_PRODUTO - 1] = '\0';
    novo->preco_unitario = prod->preco;
    novo->quantidade     = quantidade;
    novo->total_item     = prod->preco * (float)quantidade;

    /* Inserção no início da lista encadeada [LP2 — Lista Encadeada] */
    novo->proximo    = carrinho->cabeca;
    carrinho->cabeca = novo;
    carrinho->qtd_itens++;
    carrinho->total = CalculaTotalRecursivo(carrinho->cabeca);

    printf("  \"%s\" adicionado ao carrinho.\n", prod->nome);
    free(prod);
    return 1;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  CARRINHO — REMOÇÃO DE ITEM
 *  [LP2 — Duplo Ponteiro] usa ItemCarrinho** para reescrever qualquer
 *  ponteiro da lista (cabeça ou campo *proximo de um nó anterior)
 * ═══════════════════════════════════════════════════════════════════════════ */

int RemoveItemCarrinhoById(Carrinho **carrinho, int id_produto) {
    if (*carrinho == NULL || (*carrinho)->cabeca == NULL)
        return 0;

    /* ptr aponta para o campo que guarda o endereço do nó atual:
     * inicialmente é &cabeca; depois passa a ser &(anterior->proximo). */
    ItemCarrinho **ptr = &((*carrinho)->cabeca);

    while (*ptr != NULL) {
        if ((*ptr)->id_produto == id_produto) {
            ItemCarrinho *remover = *ptr;
            *ptr = remover->proximo;          /* religa a lista */
            free(remover);
            (*carrinho)->qtd_itens--;
            (*carrinho)->total = CalculaTotalRecursivo((*carrinho)->cabeca);
            return 1;
        }
        ptr = &((*ptr)->proximo);             /* avança usando duplo ponteiro */
    }
    return 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  CARRINHO — LIMPEZA TOTAL
 *  [LP2 — Duplo Ponteiro] recebe Carrinho** para zerar a estrutura original
 * ═══════════════════════════════════════════════════════════════════════════ */

void LimpaCarrinho(Carrinho *carrinho) {
    if (carrinho == NULL) return;

    ItemCarrinho *atual = carrinho->cabeca;
    while (atual != NULL) {
        ItemCarrinho *prox = atual->proximo;
        free(atual);
        atual = prox;
    }
    carrinho->cabeca    = NULL;
    carrinho->qtd_itens = 0;
    carrinho->total     = 0.0f;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  CÁLCULO RECURSIVO DO TOTAL
 *  [LP2 — Função Recursiva]
 *  Base: lista vazia → 0.0
 *  Passo: total_item do nó atual + resultado recursivo para o próximo nó
 * ═══════════════════════════════════════════════════════════════════════════ */

float CalculaTotalRecursivo(ItemCarrinho *item) {
    if (item == NULL)
        return 0.0f;
    return item->total_item + CalculaTotalRecursivo(item->proximo);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  EXIBIÇÃO DE PRODUTO — implementação padrão do tipo FuncExibeProduto
 * ═══════════════════════════════════════════════════════════════════════════ */

void ExibeProdutoPadrao(Produto *p) {
    printf("  %-5d | %-30s | R$ %-8.2f | Estoque: %d\n",
           p->id, p->nome, p->preco, p->quantidade);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  LISTAGEM DE PRODUTOS COM PONTEIRO DE FUNÇÃO
 *  [LP2 — Ponteiro de Função] 'exibe' pode ser trocado pelo chamador
 * ═══════════════════════════════════════════════════════════════════════════ */

void ListaProdutosVenda(FILE *arq_produtos, FuncExibeProduto exibe) {
    Produto p;
    int total = 0;
    rewind(arq_produtos);

    printf("  %-5s | %-30s | %-11s | %s\n", "ID", "NOME", "PRECO", "ESTOQUE");
    printf("  %s\n", "--------------------------------------------------------");

    while (fread(&p, sizeof(Produto), 1, arq_produtos) == 1) {
        if (p.ativo && p.quantidade > 0) {
            exibe(&p);   /* chamada via ponteiro de função [LP2] */
            total++;
        }
    }
    if (total == 0)
        printf("  (nenhum produto disponivel em estoque)\n");
    printf("\n");
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  EXIBIÇÃO DO CARRINHO
 * ═══════════════════════════════════════════════════════════════════════════ */

void ExibeCarrinho(Carrinho *carrinho) {
    if (carrinho->qtd_itens == 0) {
        printf("  [carrinho vazio]\n\n");
        return;
    }

    printf("  %-30s | %-5s | %-10s | %s\n",
           "PRODUTO", "QTD", "UNIT. (R$)", "TOTAL (R$)");
    printf("  %s\n", "------------------------------------------------------------");

    /* Percorre a lista encadeada [LP2 — Lista Encadeada] */
    ItemCarrinho *atual = carrinho->cabeca;
    while (atual != NULL) {
        printf("  %-30s | %-5d | %-10.2f | %.2f\n",
               atual->nome_produto,
               atual->quantidade,
               atual->preco_unitario,
               atual->total_item);
        atual = atual->proximo;
    }

    printf("  %s\n", "------------------------------------------------------------");
    printf("  TOTAL DO CARRINHO: R$ %.2f\n\n", carrinho->total);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  NOTA FISCAL
 *  Exibe o comprovante completo da venda (portado da LojaIntegrada).
 * ═══════════════════════════════════════════════════════════════════════════ */

void ImprimirNotaFiscal(Carrinho *carrinho, Pet *pet, float valor_pago) {
    float troco = valor_pago - carrinho->total;

    printf("\n");
    printf("  ===========================================================\n");
    printf("  *          NOTA FISCAL — CLINICA VETERINARIA              *\n");
    printf("  ===========================================================\n");
    if (pet->nome != NULL)
        printf("  Pet     : %s (ID: %d)\n", pet->nome, pet->id);
    else
        printf("  Pet ID  : %d\n", pet->id);
    printf("  Tutor ID: %d\n", pet->id_Cliente);
    printf("  -----------------------------------------------------------\n");

    printf("  %-30s | %-5s | %-10s | %s\n",
           "PRODUTO", "QTD", "UNIT. (R$)", "TOTAL (R$)");
    printf("  %s\n", "------------------------------------------------------------");

    ItemCarrinho *atual = carrinho->cabeca;
    while (atual != NULL) {
        printf("  %-30s | %-5d | %-10.2f | %.2f\n",
               atual->nome_produto,
               atual->quantidade,
               atual->preco_unitario,
               atual->total_item);
        atual = atual->proximo;
    }

    printf("  ===========================================================\n");
    printf("  TOTAL DA COMPRA : R$ %.2f\n", carrinho->total);
    printf("  VALOR PAGO      : R$ %.2f\n", valor_pago);
    printf("  TROCO           : R$ %.2f\n", troco);
    printf("  ===========================================================\n");
    printf("  Obrigado! Volte sempre.\n\n");
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  PERSISTÊNCIA DA VENDA
 * ═══════════════════════════════════════════════════════════════════════════ */

int SalvaRegistroVenda(RegistroVenda *venda, FILE *arq_vendas) {
    fseek(arq_vendas, 0, SEEK_END);
    if (fwrite(venda, sizeof(RegistroVenda), 1, arq_vendas) != 1) {
        perror("  ERRO ao gravar venda");
        return 0;
    }
    fflush(arq_vendas);
    return 1;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  RELATÓRIO DE VENDAS COM MATRIZ DINÂMICA
 *  [LP2 — Matriz Dinâmica] aloca float** com linhas × 6 colunas:
 *    col 0 → id_venda  | col 1 → id_pet   | col 2 → id_cliente
 *    col 3 → qtd_itens | col 4 → total    | col 5 → troco
 * ═══════════════════════════════════════════════════════════════════════════ */

void GerarRelatorioVendas(FILE *arq_vendas) {
    #define NUM_COLUNAS 6

    /* Conta registros */
    fseek(arq_vendas, 0, SEEK_END);
    long tam = ftell(arq_vendas);
    int  nLinhas = (int)(tam / (long)sizeof(RegistroVenda));

    if (nLinhas == 0) {
        printf("  Nenhuma venda registrada.\n");
        return;
    }

    /* Aloca vetor dinâmico de ponteiros (linhas) [LP2 — Vetor Dinâmico] */
    float **matriz = (float **)malloc(nLinhas * sizeof(float *));
    if (matriz == NULL) {
        printf("  ERRO: Memoria insuficiente para o relatorio.\n");
        return;
    }

    /* Aloca cada linha (vetor de NUM_COLUNAS floats) [LP2 — Matriz Dinâmica] */
    for (int i = 0; i < nLinhas; i++) {
        matriz[i] = (float *)malloc(NUM_COLUNAS * sizeof(float));
        if (matriz[i] == NULL) {
            /* Libera o que já foi alocado antes de sair */
            for (int k = 0; k < i; k++) free(matriz[k]);
            free(matriz);
            printf("  ERRO: Memoria insuficiente para o relatorio.\n");
            return;
        }
    }

    /* Preenche a matriz lendo os registros do arquivo */
    RegistroVenda reg;
    rewind(arq_vendas);
    for (int i = 0; i < nLinhas; i++) {
        if (fread(&reg, sizeof(RegistroVenda), 1, arq_vendas) != 1) break;
        matriz[i][0] = (float)reg.id_venda;
        matriz[i][1] = (float)reg.id_pet;
        matriz[i][2] = (float)reg.id_cliente;
        matriz[i][3] = (float)reg.qtd_itens;
        matriz[i][4] = reg.total;
        matriz[i][5] = reg.troco;
    }

    /* Exibe a tabela */
    printf("\n");
    printf("  ========== RELATORIO DE VENDAS ==========\n");
    printf("  %-8s | %-8s | %-10s | %-5s | %-10s | %s\n",
           "ID VENDA", "ID PET", "ID CLIENTE", "ITENS", "TOTAL(R$)", "TROCO(R$)");
    printf("  %s\n",
           "------------------------------------------------------------------");
    for (int i = 0; i < nLinhas; i++) {
        printf("  %-8.0f | %-8.0f | %-10.0f | %-5.0f | %-10.2f | %.2f\n",
               matriz[i][0], matriz[i][1], matriz[i][2],
               matriz[i][3], matriz[i][4], matriz[i][5]);
    }
    printf("  %s\n",
           "------------------------------------------------------------------");
    printf("  Total de vendas: %d\n\n", nLinhas);

    /* Libera a matriz dinâmica */
    for (int i = 0; i < nLinhas; i++) free(matriz[i]);
    free(matriz);

    #undef NUM_COLUNAS
}
