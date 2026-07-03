#include "consulta.h"
#include "pets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Consulta *MarcarConsulta(Consulta *nova) {

  printf("Dados para o Agendamento:\n");

  printf("Nome do animal: ");
  scanf("%s", nova->animal.nome);

  printf("\n valor: ");
  scanf("%f", &nova->valor);

  printf("\nData: ");
  scanf("%s", nova->data);

  printf("\nHorario: ");
  scanf("%d", &(nova->horario));

  printf("Consulta marcada com sucesso!\n");
  return nova;
}

int RealizarConsulta(Consulta *consulta) {

  int estado_animal;

  if (consulta == NULL) {
    printf("Consulta invalida.\n");
    return 0;
  }

  printf("Dados do animal:\n");
  printf("Situacao(0-10):");

  scanf("%d", &estado_animal);

  if (estado_animal <= 3) {
    consulta->animal.estado = CRITICA;
  } else if (estado_animal < 5) {
    consulta->animal.estado = GRAVE;
  } else if (estado_animal < 7) {
    consulta->animal.estado = ESTAVEL;
  } else {
    consulta->animal.estado = BOA;
  }

  printf("Consulta realizada com sucesso!\n");
  return 1;
}

int RegistrarConsulta(Consulta consulta) {
  /* Aqui entraria a logica de registrar a consulta (ex: salvar em
   * arquivo/banco) */
  printf("Consulta registrada com sucesso!\n");
  return 1;
}

void InicializarLista(ListaConsulta *lista) {
  lista->inicio = NULL;
  lista->tamanho = 0;
}

int InserirConsulta(ListaConsulta *lista, Consulta consulta) {

  NoConsulta *novo = (NoConsulta *)malloc(sizeof(NoConsulta));
  if (novo == NULL) {
    printf("Erro ao alocar memoria para o novo no.\n");
    return 0;
  }

  novo->dados = consulta;
  novo->proximo = lista->inicio;
  lista->inicio = novo;
  lista->tamanho++;

  return 1;
}

int RemoverConsulta(ListaConsulta *lista, int indice) {
  if (indice < 0 || indice >= lista->tamanho) {
    printf("Indice invalido.\n");
    return 0;
  }

  NoConsulta *atual = lista->inicio;
  NoConsulta *anterior = NULL;
  int i;

  for (i = 0; i < indice; i++) {
    anterior = atual;
    atual = atual->proximo;
  }

  if (anterior == NULL) {
    lista->inicio = atual->proximo;
  } else {
    anterior->proximo = atual->proximo;
  }

  free(atual);
  lista->tamanho--;

  return 1;
}

int VerificarConflito(ListaConsulta *lista, Consulta consulta) {
  NoConsulta *atual = lista->inicio;

  while (atual != NULL) {
    int mesmaData = (strcmp(atual->dados.data, consulta.data) == 0);
    int mesmoHorario = (atual->dados.horario == consulta.horario);

    /* Ajuste esta comparacao para o campo que identifica o veterinario
       na sua struct Funcionario, ex: .id, .cpf, .nome, etc. */
    int mesmoVeterinario =
        (atual->dados.veterinario.id == consulta.veterinario.id);

    if (mesmaData && mesmoHorario && mesmoVeterinario) {
      return 1; /* conflito encontrado */
    }

    atual = atual->proximo;
  }

  return 0;
}

Consulta *BuscarConsulta(ListaConsulta *lista, int indice) {
  if (indice < 0 || indice >= lista->tamanho) {
    return NULL;
  }

  NoConsulta *atual = lista->inicio;
  int i;

  for (i = 0; i < indice; i++) {
    atual = atual->proximo;
  }

  return &(atual->dados);
}

void LiberarLista(ListaConsulta *lista) {
  NoConsulta *atual = lista->inicio;
  NoConsulta *proximo;

  while (atual != NULL) {
    proximo = atual->proximo;
    free(atual);
    atual = proximo;
  }

  lista->inicio = NULL;
  lista->tamanho = 0;
}
