#include <stdio.h>

#include "menu_cadastro.h"
#include "menu_pet.h"
#include "menu_produto.h"
#include "menu_veterinario.h"
#include "../utils/utilidades.h"

static void imprimirMenu(void)
{
    printf("\n========== CADASTRO ==========\n");

    printf("1 - Pet\n");
    printf("2 - Produto\n");
    printf("3 - Veterinario\n");
    printf("4 - Voltar\n");
}

void MenuCadastro(void)
{
    int opcao = 0;

    while(opcao != 4)
    {
        imprimirMenu();

        opcao = getOpcao();

        switch(opcao)
        {
            case 1:

                MenuPet();

                break;

            case 2:

                MenuProduto();

                break;

            case 3:

                MenuVeterinario();

                break;

            case 4:

                break;

            default:

                printf("\nOpcao invalida.\n");
        }
    }
}