#ifndef MENU_CONSULTAS_H
#define MENU_CONSULTAS_H

/*
 * menus/menu_consultas.h — Submenu de Consultas
 *
 * Ponto de entrada do fluxo de consultas integrado ao atendimento.
 * Mantém uma ListaConsulta interna estática (em memória) e oferece
 * as seguintes opções:
 *
 *   1 - Agendar Consulta
 *   2 - Realizar Consulta
 *   3 - Cancelar Consulta
 *   4 - Atualizar Status
 *   5 - Listar Consultas
 *   6 - Voltar
 *
 * Uso em menu_atendimento.c:
 *
 *     #include "menu_consultas.h"
 *     ...
 *     case 1:
 *         MenuConsultas();
 *         break;
 */
void MenuConsultas(void);

#endif /* MENU_CONSULTAS_H */
