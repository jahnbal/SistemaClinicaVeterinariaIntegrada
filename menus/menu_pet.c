#include <stdio.h>

#include "../pets.h"
#include "../utils/utilidades.h"
#include "menu_pet.h"
static void imprimirMenuPet(void) {
  printf("\n========================================\n");
  printf("            MENU PET\n");
  printf("========================================\n");

  printf("1 - Cadastrar Pet\n");
  printf("2 - Remover Pet\n");
  printf("3 - Atualizar Pet\n");
  printf("4 - Voltar\n");
}

static void cadastrarPet(void) {
  FILE *arquivo_pets = fopen(NOME_ARQUIVO_PETS, "wb");
  Pet bicho;

  if (!arquivo_pets) {
    printf("Erro ao abrir arquivo");
  }

  IniciaPet(&bicho);
  CadastraPet(&bicho, arquivo_pets);

  fclose(arquivo_pets);
}

static void removerPet(void) {

  Pet bicho;
  FILE *arquivo_r_pets = fopen(NOME_ARQUIVO_PETS, "rb");

  if (arquivo_r_pets == NULL) {
    printf("Erro ao abrir arquivo");
  }

  printf("\n[Remover Pet]\n");
  printf("Id do pet a ser removido:");
  scanf("%d", &bicho.id);

  RemovePet(bicho, arquivo_r_pets);

  fclose(arquivo_r_pets);
}

static void atualizarPet(void) {

  Pet bicho;
  printf("\n[Atualizar Pet]\n");
  printf("Id do pet a ser atualizado\n");
  scanf("%d", &bicho.id);

  AtualizaPet(&bicho);
}

void MenuPet(void) {
  int opcao = 0;

  while (opcao != 4) {
    imprimirMenuPet();

    opcao = getOpcao();

    switch (opcao) {
    case 1:
      cadastrarPet();
      break;

    case 2:
      removerPet();
      break;

    case 3:
      atualizarPet();
      break;

    case 4:
      break;

    default:
      printf("\nOpcao invalida.\n");
    }
  }
}
