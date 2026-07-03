#include "menu_atendimento.h"
#include "menu_consultas.h"
#include "menu_venda.h"
#include "../pets.h"
#include "../utils/utilidades.h"
#include <stdio.h>
#include <string.h>

static void imprimirMenu(void) {
    printf("\n=============================\n");
    printf("    REALIZAR ATENDIMENTO\n");
    printf("=============================\n");
    printf("1 - Consultas\n");
    printf("2 - Venda\n");
    printf("3 - Voltar\n");
}

void MenuAtendimento(void) {
    int opcao = 0;

    while (opcao != 3) {
        imprimirMenu();

        opcao = getOpcao();

        switch (opcao) {

        case 1:
            /* Fluxo completo de consultas — menu_consultas.c */
            MenuConsultas();
            break;

        case 2: {
            /*
             * TODO: integrar seleção de pet via arquivo.
             * Por ora, usa pet de teste para não alterar o fluxo de vendas.
             */
            Pet petTeste;
            petTeste.id         = 1;
            petTeste.id_Cliente = 1;
            petTeste.idade      = 5;
            petTeste.peso       = 20.5f;
            petTeste.estado     = BOA;
            strncpy(petTeste.nome, "Ray", sizeof(petTeste.nome));
            petTeste.nome[sizeof(petTeste.nome) - 1] = '\0';

            MenuVendaAtendimento(&petTeste);
            break;
        }

        case 3:
            break;

        default:
            printf("\n  Opcao invalida.\n");
        }
    }
}
