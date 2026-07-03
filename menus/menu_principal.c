#include <stdio.h>

#include "menu_principal.h"
#include "menu_atendimento.h"
#include "menu_cadastro.h"
#include "menu_busca.h"

#include "../utils/utilidades.h"

static void imprimirMenuPrincipal(void)
{
    printf("\n========================================\n");
    printf(" SISTEMA CLINICA VETERINARIA\n");
    printf("========================================\n");

    printf("1 - Realizar Atendimento\n");
    printf("2 - Realizar Cadastro\n");
    printf("3 - Busca\n");
    printf("4 - Sair\n");
}

void MenuPrincipal(void)
{
    int opcao = 0;

    while(opcao != 4)
    {
        imprimirMenuPrincipal();

        opcao = getOpcao();

        switch(opcao)
        {
            case 1:
                MenuAtendimento();
                break;

            case 2:
                MenuCadastro();
                break;

            case 3:
                MenuBusca();
                break;

            case 4:
                printf("\nEncerrando sistema...\n");
                break;

            default:
                printf("\nOpcao invalida.\n");
        }
    }
}