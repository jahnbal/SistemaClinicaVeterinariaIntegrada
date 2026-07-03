#include <stdio.h>

#include "../funcionario.h"
#include "../pets.h"
#include "../produtos.h"
#include "../utils/utilidades.h"
#include "menu_busca.h"

static void imprimirMenu(void) {
  printf("\n========== BUSCA ==========\n");

  printf("1 - Buscar Pet\n");
  printf("2 - Buscar Veterinario\n");
  printf("3 - Buscar Produto\n");
  printf("4 - Voltar\n");
}

void MenuBusca(void) {

  int opcao = 0;
  int buscado;
  Funcionario funcionario;
  Funcionario *encontrado = NULL;
  Produto produto;
  Pet bicho;
  FILE *arq_produtos = fopen(ARQUIVO_PRODS, "rb");
  FILE *arq_pets = fopen(NOME_ARQUIVO_PETS, "rb");
  FILE *arq_funcs = fopen(NOME_ARQUIVO_FUNC, "rb");

  while (opcao != 4) {
    imprimirMenu();

    opcao = getOpcao();

    switch (opcao) {
    case 1:

      printf("\nBuscar Pet...\n");
      printf("Id do Pet buscado: \n");
      scanf("%d", &bicho.id);

      buscado = BuscaPet(arq_pets, bicho.id, &bicho);

      if (buscado == 1) {
        printf("pet %s encontrado!\n", bicho.nome);
      } else {
        printf("pet nao encontrado.\n");
      }

      break;

    case 2:

      printf("Digite o id do Funcionario buscado:\n");
      scanf("%d", &funcionario.id),
          encontrado = BuscarFuncionarioPorID(arq_funcs, funcionario.id);

      if (encontrado != NULL) {
        printf("Nome: %s\n", encontrado->nome);
        printf("Ocupacao: %d\n", encontrado->ocupacao);
      } else {
        printf("Funcionario nao encontrado.\n");
      }
      break;

    case 3:

      printf("\nBuscar Produto...\n");
      printf("Id do Produto buscado:");
      scanf("%d", &produto.id);

      VerificaProduto(&produto, arq_produtos);
      break;

    case 4:

      break;

    default:

      printf("\nOpcao invalida.\n");
    }
  }
}
