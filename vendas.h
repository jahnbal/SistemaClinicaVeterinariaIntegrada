#ifndef VENDAS_H
#define VENDAS_H
#include "produtos.h"

int RealizarVenda(Produto produto);
void RegistrarVenda(Produto produto);
int BuscarVenda(Produto produto, FILE *arq);

#endif
