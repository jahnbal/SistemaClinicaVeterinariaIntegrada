#ifndef FUNCIONARIO_H
#define FUNCIONARIO_H

#include <stdio.h>

#define NOME_ARQUIVO_FUNC "Funcs.dat"
#define QTDE_FUNCS 6

typedef struct {

  int id;
  char nome[50];
  int ocupacao;
  int horarios[5];

} Funcionario;

void CadastroFunc(FILE *arq);
void RemoveFunc(Funcionario *func);
int AlteraFunc(int id, const char *novoNome, int novaOcupacao,
               int *novosHorarios, FILE *arq);

Funcionario *BuscarFuncionarioPorID(FILE *arq, int id);

#endif
