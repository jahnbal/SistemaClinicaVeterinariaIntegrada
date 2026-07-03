#include "pets.h"
#include <stdio.h>
#include <string.h>

void IniciaPet(Pet *bicho) {

  int c;

  printf("Td do pet: \n");
  scanf("%d", &bicho->id);

  printf("Id do tutor: \n");
  scanf("%d", &bicho->id_Cliente);

  printf("Nome do Pet: \n");
  scanf(" %s", bicho->nome);

  while ((c = getchar()) != '\n')
    ;

  printf("Idade do Pet: \n");
  scanf("%d", &bicho->idade);

  printf("Peso do Pet: \n");
  scanf("%f", &bicho->peso);

  while ((c = getchar()) != '\n')
    ;
}
int BuscaPet(FILE *arq, int id, Pet *bicho) {
  if (arq == NULL || bicho == NULL) {
    printf("Erro: arquivo ou ponteiro invalido.\n");
    return 0;
  }

  rewind(arq); /* garante que a leitura comeca do inicio do arquivo */

  while (fread(bicho, sizeof(Pet), 1, arq) == 1) {
    if (bicho->id == id) {
      return 1; /* encontrado, bicho ja preenchido */
    }
  }

  return 0; /* nao encontrado */
}
void CadastraPet(Pet *bicho, FILE *arq) {
  if (arq == NULL) {
    printf("Erro: arquivo invalido.\n");
    return;
  }

  fseek(arq, 0, SEEK_END);
  if (fwrite(bicho, sizeof(Pet), 1, arq) != 1) {
    printf("Erro ao cadastrar o pet '%s'.\n", bicho->nome);
    return;
  }

  fflush(arq);
  printf("Pet '%s' (id: %d) cadastrado com sucesso!\n", bicho->nome, bicho->id);
}

int RemovePet(Pet bicho, FILE *arq) {
  FILE *temp;
  Pet aux;
  int removido = 0;

  if (arq == NULL) {
    printf("Erro: arquivo invalido.\n");
    return -1;
  }

  temp = fopen("pets_temp.dat", "wb");
  if (temp == NULL) {
    printf("Erro ao criar arquivo temporario.\n");
    return -1;
  }

  rewind(arq);
  while (fread(&aux, sizeof(Pet), 1, arq) == 1) {
    if (aux.id == bicho.id && !removido) {
      removido = 1;
      continue; /* pula o pet a ser removido */
    }
    fwrite(&aux, sizeof(Pet), 1, temp);
    fclose(temp);

    if (!removido) {
      printf("Pet com id %d nao encontrado.\n", bicho.id);
      remove("pets_temp.dat");
      return 0;
    }

    fclose(arq);

    if (remove(NOME_ARQUIVO_PETS) != 0) {
      printf("Erro ao remover arquivo original.\n");
      return 0;
    }

    if (rename("pets_temp.dat", "pets.dat") != 0) {
      printf("Erro ao renomear arquivo temporario.\n");
      return 0;
    }

    printf("Pet '%s' (id %d) removido com sucesso!\n", bicho.nome, bicho.id);
    return 1;
  }
  return 2;
}

void AtualizaPet(Pet *bicho) {

  FILE *arquivo_at_pets = fopen(NOME_ARQUIVO_PETS, "rb");
  int remocao;
  remocao = RemovePet(*bicho, arquivo_at_pets);

  if (remocao == 0 || remocao == -1) {
    return;
  }

  int opcao;
  int est;

  if (bicho == NULL) {
    printf("Erro: pet invalido.\n");
    return;
  }

  printf("\nAtualizando dados de '%s' (id %d):\n", bicho->nome, bicho->id);
  printf("1 - Nome\n");
  printf("2 - Idade\n");
  printf("3 - Peso\n");
  printf("4 - ID do cliente\n");
  printf("0 - Cancelar\n");
  printf("Escolha o campo a atualizar: ");
  scanf("%d", &opcao);

  switch (opcao) {
  case 1:
    printf("Novo nome: ");
    scanf(" %49[^\n]", bicho->nome);
    break;

  case 2:
    printf("Nova idade: ");
    scanf("%d", &bicho->idade);
    break;

  case 3:
    printf("Novo peso: ");
    scanf("%f", &bicho->peso);
    break;

  case 4:
    printf("Novo ID do cliente: ");
    scanf("%d", &bicho->id_Cliente);
    break;

  case 0:
    printf("Atualizacao cancelada.\n");
    return;

  default:
    printf("Opcao invalida.\n");
    return;
  }

  FILE *arquivo_novo_pets = fopen(NOME_ARQUIVO_PETS, "wb");

  printf("Pet '%s' atualizado com sucesso!\n", bicho->nome);

  CadastraPet(bicho, arquivo_novo_pets);
}
