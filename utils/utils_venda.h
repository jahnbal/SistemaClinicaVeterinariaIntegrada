/*
 * utils/utils_venda.h — Utilitários de I/O para o módulo de vendas
 *
 * Se o projeto já possuir funções equivalentes em outro arquivo da pasta
 * utils/, basta remover as declarações duplicadas e ajustar o #include
 * no menu_venda.c.
 */

#ifndef UTILS_VENDA_H
#define UTILS_VENDA_H

/* Limpa o terminal (funciona em Windows e Unix/Linux) */
void limpar_tela_vet(void);

/* Aguarda o usuário pressionar ENTER para continuar */
void aguardar_enter_vet(void);

/* Leitura segura de inteiro: repete até o usuário digitar um número válido */
int ler_inteiro_vet(void);

/* Leitura segura de float positivo */
float ler_float_positivo_vet(void);

#endif /* UTILS_VENDA_H */
