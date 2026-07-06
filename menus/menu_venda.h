#ifndef MENU_VENDA_H
#define MENU_VENDA_H

#include "../pets.h"

/*
  Ponto de entrada pro fluxo de vendas integrado ao atendimento
  Abre os arquivos necessários, exibe o menu interativo e libera
  todos os recursos ao terminar
 */
void MenuVendaAtendimento(Pet *pet);

/*
  Exibe o histórico de todas as vendas registradas em vendas.dat,
  usando a matriz dinâmica de GerarRelatorioVendas().
  Pode ser chamado de qualquer menu
 */
void MenuRelatorioVendas(void);

#endif
