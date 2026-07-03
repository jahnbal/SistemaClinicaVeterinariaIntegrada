#ifndef PETS_H
#define PETS_H

#include <stdio.h>

#define NOME_ARQUIVO_PETS "pets.dat"

typedef enum {

  BOA,
  ESTAVEL,
  GRAVE,
  CRITICA,

} Situacao;

typedef struct {

  char nome[30];
  int id;
  int idade;
  float peso;
  Situacao estado;
  int id_Cliente;

} Pet;

void IniciaPet(Pet *bicho);
int BuscaPet(FILE *arq, int id, Pet *bicho);
void CadastraPet(Pet *bicho, FILE *arq);
int RemovePet(Pet bicho);
void AtualizaPet(Pet *bicho);

#endif
