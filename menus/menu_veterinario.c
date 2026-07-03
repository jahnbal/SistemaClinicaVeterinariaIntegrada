#include <stdio.h>
#include <string.h>

#include "../funcionario.h"
#include "../utils/utilidades.h"
#include "menu_veterinario.h"

static FILE *abrirArquivoVeterinario(void) {
  FILE *arq = fopen(NOME_ARQUIVO_FUNC, "rb+");
  if (arq == NULL) {
    arq = fopen(NOME_ARQUIVO_FUNC, "ab+");
    if (arq == NULL) {
      perror("Erro ao abrir o arquivo de veterinarios");
      return NULL;
    }
  }

  return arq;
}

static void imprimirMenuVeterinario(void) {
  printf("\n========================================\n");
  printf("        MENU VETERINARIO\n");
  printf("========================================\n");

  printf("1 - Cadastrar Veterinario\n");
  printf("2 - Remover Veterinario\n");
  printf("3 - Atualizar Veterinario\n");
  printf("4 - Voltar\n");
}

static void cadastrarVeterinario(void) {
  FILE *arqVeterinario = abrirArquivoVeterinario();
  if (arqVeterinario == NULL) {
    printf("Nao foi possivel acessar o arquivo '%s'.\n", NOME_ARQUIVO_FUNC);
    return;
  }

  printf("\n[Cadastrar Veterinario]\n");
  CadastroFunc(arqVeterinario);
  fclose(arqVeterinario);
}

static void removerVeterinario(void) {
  FILE *arqVeterinario = abrirArquivoVeterinario();

  if (arqVeterinario == NULL) {
    printf("Erro ao abrir o arquivo de veterinarios.\n");
    return;
  }

  int id;

  printf("\n========== REMOVER VETERINARIO ==========\n\n");

  printf("Informe o ID do veterinario: ");
  scanf("%d", &id);

  Funcionario *vet = BuscarFuncionarioPorID(arqVeterinario, id);

  if (vet == NULL) {
    printf("\nVeterinario nao encontrado.\n");
    fclose(arqVeterinario);
    return;
  }

  printf("\nVeterinario encontrado:\n");
  printf("ID: %d\n", vet->id);
  printf("Nome: %s\n", vet->nome);

  char opcao;

  printf("\nDeseja realmente remover este veterinario? (S/N): ");
  scanf(" %c", &opcao);

  if (opcao == 'S' || opcao == 's') {
    fclose(arqVeterinario);
    RemoveFunc(vet);
  } else {
    fclose(arqVeterinario);
    printf("\nOperacao cancelada.\n");
  }
}

static void atualizarVeterinario(void) {
  FILE *arqVeterinario = abrirArquivoVeterinario();

  if (arqVeterinario == NULL) {
    printf("Erro ao abrir o arquivo de veterinarios.\n");
    return;
  }

  int id;
  char novoNome[50];
  int novaOcupacao;
  char opcao;

  printf("\n========== ATUALIZAR VETERINARIO ==========\n\n");

  printf("Informe o ID do veterinario: ");
  scanf("%d", &id);
  getchar();

  Funcionario *vet = BuscarFuncionarioPorID(arqVeterinario, id);

  if (vet == NULL) {
    printf("\nVeterinario nao encontrado.\n");
    fclose(arqVeterinario);
    return;
  }

  printf("\nVeterinario encontrado:\n");
  printf("ID: %d\n", vet->id);
  printf("Nome: %s\n", vet->nome);
  printf("Ocupacao: %d\n", vet->ocupacao);

  printf("\nNovo nome: ");
  fgets(novoNome, sizeof(novoNome), stdin);
  novoNome[strcspn(novoNome, "\n")] = '\0';

  printf("Nova ocupacao: ");
  scanf("%d", &novaOcupacao);

  printf("\nConfirmar alteracoes? (S/N): ");
  scanf(" %c", &opcao);

  if (opcao == 'S' || opcao == 's') {
    AlteraFunc(vet->id, novoNome, novaOcupacao, NULL, arqVeterinario);
  } else {
    printf("\nOperacao cancelada.\n");
  }

  fclose(arqVeterinario);
}

void MenuVeterinario(void) {
  int opcao = 0;

  while (opcao != 4) {
    imprimirMenuVeterinario();

    opcao = getOpcao();

    switch (opcao) {
    case 1:
      cadastrarVeterinario();
      break;

    case 2:
      removerVeterinario();
      break;

    case 3:
      atualizarVeterinario();
      break;

    case 4:
      break;

    default:
      printf("\nOpcao invalida.\n");
    }
  }
}
