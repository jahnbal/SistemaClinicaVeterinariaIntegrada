/*
 * utils/utils_venda.c — Implementação dos utilitários de I/O
 *
 * Portado e adaptado das funções de utilidades.c da LojaIntegrada.
 */

#include "utils_venda.h"
#include <stdio.h>
#include <stdlib.h>

/* ─────────────────────────────────────────────────────────────────────────
 * limpar_tela_vet — limpa o terminal de forma portável
 * ───────────────────────────────────────────────────────────────────────── */
void limpar_tela_vet(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* ─────────────────────────────────────────────────────────────────────────
 * aguardar_enter_vet — pausa até o usuário pressionar ENTER
 * Descarta qualquer caractere restante no buffer antes de aguardar.
 * ───────────────────────────────────────────────────────────────────────── */
void aguardar_enter_vet(void) {
    int c;
    /* Descarta resíduo do buffer */
    while ((c = getchar()) != '\n' && c != EOF);
    printf("\n  Pressione ENTER para continuar...");
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ─────────────────────────────────────────────────────────────────────────
 * ler_inteiro_vet — leitura segura de inteiro via scanf
 * Repete em caso de entrada inválida, limpando o buffer a cada tentativa.
 * ───────────────────────────────────────────────────────────────────────── */
int ler_inteiro_vet(void) {
    int valor;
    int c;
    while (scanf("%d", &valor) != 1) {
        printf("  Entrada invalida. Digite um numero inteiro: ");
        while ((c = getchar()) != '\n' && c != EOF);
    }
    /* Limpa o '\n' residual */
    while ((c = getchar()) != '\n' && c != EOF);
    return valor;
}

/* ─────────────────────────────────────────────────────────────────────────
 * ler_float_positivo_vet — leitura segura de float (>= 0)
 * ───────────────────────────────────────────────────────────────────────── */
float ler_float_positivo_vet(void) {
    float valor;
    int c;
    for (;;) {
        if (scanf("%f", &valor) == 1 && valor >= 0.0f)
            break;
        printf("  Valor invalido (deve ser >= 0). Tente novamente: ");
        while ((c = getchar()) != '\n' && c != EOF);
    }
    while ((c = getchar()) != '\n' && c != EOF);
    return valor;
}
