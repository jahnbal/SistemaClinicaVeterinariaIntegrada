/*
 * menus/menu_venda.c — Implementação do menu de vendas no atendimento
 *
 * Integra o fluxo de venda da LojaIntegrada ao atendimento veterinário:
 *  • O pet já está identificado (recebido por parâmetro).
 *  • Produtos são buscados no arquivo produtos.dat.
 *  • O carrinho usa lista encadeada (LP2).
 *  • Ao finalizar, desconta estoque e grava o registro em vendas.dat.
 */

#include "menu_venda.h"
#include "../utils/utils_venda.h"
#include "../venda.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ═══════════════════════════════════════════════════════════════════════════
 *  Funções auxiliares estáticas (visíveis apenas neste arquivo)
 * ═══════════════════════════════════════════════════════════════════════════
 */

static void cabecalho_venda(Pet *pet) {
  limpar_tela_vet();
  printf("  ===========================================================\n");
  printf("  *     VENDA DURANTE ATENDIMENTO — CLINICA VETERINARIA     *\n");
  printf("  ===========================================================\n");
  if (pet->nome != NULL)
    printf("  Pet: %-20s | ID: %-5d | Tutor ID: %d\n", pet->nome, pet->id,
           pet->id_Cliente);
  else
    printf("  Pet ID: %-5d | Tutor ID: %d\n", pet->id, pet->id_Cliente);
  printf("  ===========================================================\n\n");
}

static void exibir_menu_opcoes(void) {
  printf("  -----------------------------------------------\n");
  printf("  [1] Adicionar produto ao carrinho\n");
  printf("  [2] Remover produto do carrinho\n");
  printf("  [3] Ver carrinho detalhado\n");
  printf("  [4] Finalizar compra e imprimir nota fiscal\n");
  printf("  [0] Cancelar venda e voltar ao atendimento\n");
  printf("  -----------------------------------------------\n");
  printf("  Opcao: ");
}

/* ─────────────────────────────────────────────────────────────────────────
 * Desconta o estoque de todos os itens do carrinho após a venda
 * ───────────────────────────────────────────────────────────────────────── */
static void descontar_estoque_carrinho(Carrinho *carrinho, FILE *arq_produtos) {

  int desconta;
  ItemCarrinho *atual = carrinho->cabeca;
  while (atual != NULL) {
    desconta =
        DescontaEstoque(atual->id_produto, atual->quantidade);
    atual = atual->proximo;
  }
  if (desconta == 0) {
    printf("Estoque insuficiente");
  }
}

/* ─────────────────────────────────────────────────────────────────────────
 * Monta e grava o RegistroVenda a partir do carrinho finalizado
 * ───────────────────────────────────────────────────────────────────────── */
static void gravar_registro_venda(Carrinho *carrinho, Pet *pet,
                                  float valor_pago, FILE *arq_vendas) {
  RegistroVenda reg;
  memset(&reg, 0, sizeof(RegistroVenda));

  /* ID sequencial baseado no tamanho do arquivo */
  fseek(arq_vendas, 0, SEEK_END);
  long tam = ftell(arq_vendas);
  reg.id_venda = (int)(tam / (long)sizeof(RegistroVenda)) + 1;
  reg.id_pet = pet->id;
  reg.id_cliente = pet->id_Cliente;
  reg.total = carrinho->total;
  reg.valor_pago = valor_pago;
  reg.troco = valor_pago - carrinho->total;
  reg.qtd_itens = 0;

  /* Popula os vetores paralelos de itens [LP2 — Vetores Dinâmicos] */
  ItemCarrinho *atual = carrinho->cabeca;
  while (atual != NULL && reg.qtd_itens < MAX_ITENS_NOTA) {
    int i = reg.qtd_itens;
    reg.ids_produtos[i] = atual->id_produto;
    reg.quantidades[i] = atual->quantidade;
    reg.precos[i] = atual->preco_unitario;
    reg.qtd_itens++;
    atual = atual->proximo;
  }

  if (SalvaRegistroVenda(&reg, arq_vendas))
    printf("  Venda registrada com sucesso! (ID da venda: %d)\n", reg.id_venda);
  else
    printf("  AVISO: Nao foi possivel salvar o registro da venda.\n");
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  CASE 1 — Adicionar produto
 * ═══════════════════════════════════════════════════════════════════════════
 */
static void opcao_adicionar(Carrinho *carrinho, FILE *arq_produtos) {
  printf("\n  ID do produto: ");
  int id = ler_inteiro_vet();
  if (id <= 0) {
    printf("  ID invalido.\n");
    aguardar_enter_vet();
    return;
  }
  printf("  Quantidade   : ");
  int qtd = ler_inteiro_vet();
  if (qtd <= 0) {
    printf("  Quantidade deve ser maior que zero.\n");
  } else {
    AdicionaItemCarrinho(carrinho, id, qtd, arq_produtos);
  }
  aguardar_enter_vet();
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  CASE 2 — Remover produto
 * ═══════════════════════════════════════════════════════════════════════════
 */
static void opcao_remover(Carrinho *carrinho) {
  if (carrinho->qtd_itens == 0) {
    printf("\n  O carrinho ja esta vazio.\n");
    aguardar_enter_vet();
    return;
  }
  printf("\n  ID do produto a remover: ");
  int id = ler_inteiro_vet();
  if (RemoveItemCarrinhoById(&carrinho, id))
    printf("  Item removido do carrinho.\n");
  else
    printf("  Produto ID %d nao encontrado no carrinho.\n", id);
  aguardar_enter_vet();
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  CASE 4 — Finalizar compra
 *  Portado de realizarVenda() da LojaIntegrada e adaptado:
 *    • Não precisa selecionar cliente (já definido pelo pet em atendimento).
 *    • Desconta estoque ao confirmar.
 *    • Grava RegistroVenda em arquivo.
 *  Retorna 1 se a venda foi concluída (encerra o loop do menu).
 * ═══════════════════════════════════════════════════════════════════════════
 */
static int opcao_finalizar(Carrinho *carrinho, Pet *pet, FILE *arq_produtos,
                           FILE *arq_vendas) {
  int desconto;

  if (carrinho->qtd_itens == 0) {
    printf("\n  Carrinho vazio. Adicione pelo menos um produto antes de "
           "finalizar.\n");
    aguardar_enter_vet();
    return 0;
  }

  limpar_tela_vet();
  printf("  ===========================================================\n");
  printf("  *                   CONFIRMAR VENDA                       *\n");
  printf("  ===========================================================\n\n");
  ExibeCarrinho(carrinho);

  /* ── Pagamento (portado da LojaIntegrada) ── */
  float valor_pago = 0.0f;
  do {
    printf("  Valor pago (R$): ");
    valor_pago = ler_float_positivo_vet();
    if (valor_pago < carrinho->total)
      printf("  Valor insuficiente! Faltam R$ %.2f\n",
             carrinho->total - valor_pago);
  } while (valor_pago < carrinho->total);

  descontar_estoque_carrinho(carrinho, arq_produtos);

  /* ── Nota fiscal ── */
  ImprimirNotaFiscal(carrinho, pet, valor_pago);

  /* ── Desconto de estoque e persistência ── */

  if (arq_vendas != NULL)
    gravar_registro_venda(carrinho, pet, valor_pago, arq_vendas);

  aguardar_enter_vet();
  return 1; /* sinaliza que o menu deve encerrar */
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  MenuVendaAtendimento — ponto de entrada público
 * ═══════════════════════════════════════════════════════════════════════════
 */
void MenuVendaAtendimento(Pet *pet) {
  if (pet == NULL) {
    printf("  ERRO: Nenhum pet em atendimento.\n");
    return;
  }

  /* Abre arquivo de produtos (leitura e escrita para desconto de estoque) */
  FILE *arq_produtos = fopen(ARQUIVO_PRODS, "rb+");
  if (arq_produtos == NULL) {
    printf("  ERRO: Nao foi possivel abrir '%s'.\n", ARQUIVO_PRODS);
    printf("  Verifique se existem produtos cadastrados.\n");
    aguardar_enter_vet();
    return;
  }

  /* Abre/cria arquivo de vendas */
  FILE *arq_vendas = fopen(NOME_ARQUIVO_VENDAS, "ab");
  if (arq_vendas == NULL)
    arq_vendas = fopen(NOME_ARQUIVO_VENDAS, "wb");
  /* Se ainda NULL, a venda prossegue sem persistência */

  Carrinho carrinho;
  InicializaCarrinho(&carrinho);

  int opcao;
  int venda_concluida = 0;

  while (!venda_concluida) {
    cabecalho_venda(pet);

    /* Exibe produtos com ponteiro de função padrão [LP2] */
    printf("  --- PRODUTOS DISPONIVEIS ---\n");
    ListaProdutosVenda(arq_produtos, ExibeProdutoPadrao);

    /* Exibe carrinho atual (lista encadeada) */
    printf("  --- CARRINHO ATUAL ---\n");
    ExibeCarrinho(&carrinho);

    exibir_menu_opcoes();
    opcao = ler_inteiro_vet();

    switch (opcao) {
    case 1:
      opcao_adicionar(&carrinho, arq_produtos);
      break;

    case 2:
      opcao_remover(&carrinho);
      break;

    case 3:
      limpar_tela_vet();
      printf("  --- CARRINHO DETALHADO ---\n");
      ExibeCarrinho(&carrinho);
      aguardar_enter_vet();
      break;

    case 4:
      venda_concluida =
          opcao_finalizar(&carrinho, pet, arq_produtos, arq_vendas);
      break;

    case 0:
      if (carrinho.qtd_itens > 0) {
        printf("\n  Cancelar a venda? O carrinho sera descartado. (1=Sim / "
               "0=Nao): ");
        int conf = ler_inteiro_vet();
        if (conf == 1)
          venda_concluida = 1;
      } else {
        venda_concluida = 1;
      }
      break;

    default:
      printf("  Opcao invalida.\n");
      aguardar_enter_vet();
    }
  }

  /* Libera todos os nós da lista encadeada [LP2 — Duplo Ponteiro] */
  LimpaCarrinho(&carrinho);

  fclose(arq_produtos);
  if (arq_vendas != NULL)
    fclose(arq_vendas);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  MenuRelatorioVendas — exibe histórico de vendas (matriz dinâmica)
 * ═══════════════════════════════════════════════════════════════════════════
 */
void MenuRelatorioVendas(void) {
  limpar_tela_vet();
  printf("  ===========================================================\n");
  printf("  *              HISTORICO DE VENDAS                        *\n");
  printf("  ===========================================================\n\n");

  FILE *arq_vendas = fopen(NOME_ARQUIVO_VENDAS, "rb");
  if (arq_vendas == NULL) {
    printf("  Nenhuma venda registrada ainda.\n");
    aguardar_enter_vet();
    return;
  }

  /* Chama GerarRelatorioVendas que usa matriz dinâmica [LP2] */
  GerarRelatorioVendas(arq_vendas);
  fclose(arq_vendas);

  aguardar_enter_vet();
}
