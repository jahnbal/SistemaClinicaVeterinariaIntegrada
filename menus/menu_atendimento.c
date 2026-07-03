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

static int SelecionarPet(Pet *pet)
{
    FILE *arq = fopen(NOME_ARQUIVO_PETS, "rb");

    if (arq == NULL)
    {
        printf("Erro ao abrir o arquivo de pets.\n");
        return 0;
    }

    int id;

    printf("\nInforme o ID do pet: ");
    scanf("%d", &id);

    if (!BuscaPet(arq, id, pet))
    {
        printf("Pet nao encontrado.\n");
        fclose(arq);
        return 0;
    }

    fclose(arq);
    return 1;
}

void MenuAtendimento(void) {
    int opcao = 0;

    while (opcao != 3) {
        imprimirMenu();

        opcao = getOpcao();

        switch (opcao) {

        case 1:
            // Fluxo completo de consultas
            MenuConsultas();
            break;

        case 2: {
    
            Pet pet;

            if (SelecionarPet(&pet))
            {
                MenuVendaAtendimento(&pet);
            }
            break;
        }

        case 3:
            break;

        default:
            printf("\n  Opcao invalida.\n");
        }
    }
}
