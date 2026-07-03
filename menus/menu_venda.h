/*
 * menus/menu_venda.h — Menu de Vendas durante o Atendimento
 *
 * Como usar:
 *   No seu menu_atendimento.c (ou onde o atendimento é gerenciado),
 *   inclua este header e chame MenuVendaAtendimento() passando o
 *   ponteiro para o Pet que está sendo atendido:
 *
 *       #include "menu_venda.h"
 *       ...
 *       case OPCAO_VENDA:
 *           MenuVendaAtendimento(pet_em_atendimento);
 *           break;
 */

#ifndef MENU_VENDA_H
#define MENU_VENDA_H

#include "../pets.h"

/*
 * Ponto de entrada do fluxo de vendas integrado ao atendimento.
 * Abre os arquivos necessários, exibe o menu interativo e libera
 * todos os recursos ao terminar.
 *
 * @param pet  Ponteiro para o Pet que está sendo atendido.
 *             Deve ser não-NULL e ter id_Cliente válido.
 */
void MenuVendaAtendimento(Pet *pet);

/*
 * Exibe o histórico de todas as vendas registradas em vendas.dat,
 * usando a matriz dinâmica de GerarRelatorioVendas().
 * Pode ser chamado de qualquer menu do sistema.
 */
void MenuRelatorioVendas(void);

#endif /* MENU_VENDA_H */
