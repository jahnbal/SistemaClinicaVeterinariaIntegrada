/*
 * menus/menu_consultas.c — Módulo de Consultas
 *
 * Implementa o fluxo completo do submenu "Consultas" conforme especificação
 * funcional do ProjetoLP2.
 *
 * Conceitos LP2 demonstrados neste arquivo:
 *   - Lista Encadeada   : ListaConsulta (NoConsulta*)
 *   - Alocação Dinâmica : InserirConsulta (malloc em consulta.c) +
 *                         vetor de ponteiros em listarConsultas()
 *   - Vetor Dinâmico    : arr[] em listarConsultas() para ordenação
 *
 * Funções reaproveitadas de outros módulos:
 *   consulta.c   → InicializarLista, InserirConsulta, RemoverConsulta,
 *                  VerificarConflito, ReallizarConsulta, RegistrarConsulta
 *   pets.c       → BuscaPet, IniciaPet, CadastraPet
 *   funcionario.c → BuscarFuncionarioPorID
 */

#include "menu_consultas.h"

#include "../consulta.h"
#include "../funcionario.h"
#include "../pets.h"
#include "../utils/utilidades.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================================================
 * Estado interno do módulo
 * ========================================================= */

/* Lista encadeada de consultas — persiste durante toda a execução */
static ListaConsulta lista;
static int           inicializado  = 0;
static int           proximo_id    = 1;

/* Garante que a lista seja inicializada uma única vez */
static void garantirInicializacao(void) {
    if (!inicializado) {
        InicializarLista(&lista);
        inicializado = 1;
    }
}

/* =========================================================
 * Helpers de exibição
 * ========================================================= */

/* Converte o enum StatusConsulta em string legível */
static const char *statusParaString(StatusConsulta s) {
    switch (s) {
        case AGENDADA:       return "AGENDADA";
        case EM_ATENDIMENTO: return "EM ATENDIMENTO";
        case FINALIZADA:     return "FINALIZADA";
        case CANCELADA:      return "CANCELADA";
        default:             return "DESCONHECIDO";
    }
}

/* Cabeçalho da tabela de consultas */
static void imprimirCabecalhoConsultas(void) {
    printf("  %-4s | %-18s | %-18s | %-12s | %-7s | %s\n",
           "ID", "PET", "VETERINARIO", "DATA", "HORARIO", "STATUS");
    printf("  ----+-");
    printf("-------------------+-");
    printf("-------------------+-");
    printf("-------------+-");
    printf("--------+-");
    printf("---------------\n");
}

/* Exibe uma única linha de consulta formatada */
static void imprimirLinhaConsulta(const Consulta *c) {
    printf("  %-4d | %-18s | %-18s | %-12s | %5dh    | %s\n",
           c->id,
           c->animal.nome,
           c->veterinario.nome,
           c->data,
           c->horario,
           statusParaString(c->status));
}

/* Lista todos os veterinários do arquivo binário */
static void listarVeterinarios(FILE *arq) {
    Funcionario vet;
    int total = 0;

    rewind(arq);
    printf("\n  Veterinarios cadastrados:\n");
    printf("  %-5s | %s\n", "ID", "NOME");
    printf("  -----+----------------------------------\n");
    while (fread(&vet, sizeof(Funcionario), 1, arq) == 1) {
        printf("  %-5d | %s\n", vet.id, vet.nome);
        total++;
    }
    if (total == 0)
        printf("  (nenhum veterinario cadastrado)\n");
    printf("\n");
}

/* =========================================================
 * Helpers de busca interna
 * ========================================================= */

/*
 * Percorre a lista e retorna ponteiro direto para o dado do nó
 * cuja consulta tenha o id informado. Retorna NULL se não encontrar.
 */
static Consulta *buscarPorId(int id) {
    NoConsulta *atual = lista.inicio;
    while (atual != NULL) {
        if (atual->dados.id == id)
            return &(atual->dados);
        atual = atual->proximo;
    }
    return NULL;
}

/*
 * Retorna o índice 0-based do nó com o id informado.
 * Necessário para passar a RemoverConsulta(), que opera por índice.
 * Retorna -1 se não encontrar.
 */
static int buscarIndicePorId(int id) {
    NoConsulta *atual = lista.inicio;
    int indice = 0;
    while (atual != NULL) {
        if (atual->dados.id == id)
            return indice;
        atual = atual->proximo;
        indice++;
    }
    return -1;
}

/* =========================================================
 * Helper de cadastro de pet (reutiliza IniciaPet + CadastraPet)
 * ========================================================= */

/*
 * Coleta os dados do pet via IniciaPet() e persiste no arquivo.
 * Abre o arquivo em modo "ab" para não apagar registros existentes.
 * Retorna 1 em sucesso, 0 em falha.
 */
static int cadastrarPetNoArquivo(Pet *bicho) {
    FILE *arq = fopen(NOME_ARQUIVO_PETS, "ab");
    if (arq == NULL) {
        printf("  Erro ao abrir o arquivo de pets.\n");
        return 0;
    }
    IniciaPet(bicho);
    CadastraPet(bicho, arq);
    fclose(arq);
    return 1;
}

/* =========================================================
 * Helper de comparação de datas (para ordenação)
 * ========================================================= */

/*
 * Compara duas datas no formato DD/MM/AAAA.
 * Retorna  < 0 se d1 é anterior a d2,
 *            0 se iguais,
 *          > 0 se d1 é posterior a d2.
 */
static int compararData(const char *d1, const char *d2) {
    int dia1 = 0, mes1 = 0, ano1 = 0;
    int dia2 = 0, mes2 = 0, ano2 = 0;

    sscanf(d1, "%d/%d/%d", &dia1, &mes1, &ano1);
    sscanf(d2, "%d/%d/%d", &dia2, &mes2, &ano2);

    if (ano1 != ano2) return ano1 - ano2;
    if (mes1 != mes2) return mes1 - mes2;
    return dia1 - dia2;
}

/* =========================================================
 * Opção 1 — Agendar Consulta
 * ========================================================= */
static void agendarConsulta(void) {
    Consulta nova;
    FILE    *arq_pets  = NULL;
    FILE    *arq_funcs = NULL;
    int      opcao;

    memset(&nova, 0, sizeof(Consulta));

    printf("\n=============================\n");
    printf("      AGENDAR CONSULTA\n");
    printf("=============================\n");

    /* --------------------------------------------------
     * Etapa 1 — Pet
     * -------------------------------------------------- */
    printf("\nO pet ja esta cadastrado?\n");
    printf("1 - Sim\n");
    printf("2 - Nao\n");
    opcao = getOpcao();

    if (opcao == 1) {
        int id_pet;
        printf("Informe o ID do Pet: ");
        scanf("%d", &id_pet);
        getchar();

        arq_pets = fopen(NOME_ARQUIVO_PETS, "rb");
        if (arq_pets == NULL) {
            printf("  Erro ao abrir o arquivo de pets.\n");
            return;
        }
        int encontrado = BuscaPet(arq_pets, id_pet, &nova.animal);
        fclose(arq_pets);
        arq_pets = NULL;

        if (!encontrado) {
            printf("  Pet nao encontrado.\n");
            printf("\nDeseja cadastra-lo?\n");
            printf("1 - Sim\n");
            printf("2 - Nao\n");
            int sub = getOpcao();
            if (sub == 1) {
                printf("\n  Cadastro do novo pet:\n");
                if (!cadastrarPetNoArquivo(&nova.animal)) {
                    printf("  Agendamento cancelado.\n");
                    return;
                }
            } else {
                printf("  Agendamento cancelado.\n");
                return;
            }
        }
    } else {
        /* Fluxo de cadastro do pet durante o agendamento */
        printf("\n  Cadastro do novo pet:\n");
        if (!cadastrarPetNoArquivo(&nova.animal)) {
            printf("  Agendamento cancelado.\n");
            return;
        }
    }

    /* --------------------------------------------------
     * Etapa 2 — Veterinário
     * -------------------------------------------------- */
    arq_funcs = fopen(NOME_ARQUIVO_FUNC, "rb");
    if (arq_funcs == NULL) {
        printf("  Erro ao abrir o arquivo de veterinarios.\n");
        return;
    }

    listarVeterinarios(arq_funcs);

    {
        Funcionario *vet = NULL;
        int id_vet;

        do {
            printf("Informe o ID do veterinario: ");
            printf(">> Aperte 0 para cancelar\n ");
            scanf("%d", &id_vet);
            if (id_vet == 0) {
                printf("  Agendamento cancelado.\n");
                fclose(arq_funcs);
                return;
            }
            getchar();
            vet = BuscarFuncionarioPorID(arq_funcs, id_vet);
            if (vet == NULL)
                printf("  Veterinario nao encontrado. Tente novamente.\n");
        } while (vet == NULL);

        nova.veterinario = *vet;  /* cópia local — BuscarFuncionarioPorID usa static */
    }

    fclose(arq_funcs);
    arq_funcs = NULL;

    /* --------------------------------------------------
     * Etapa 3 — Data
     * -------------------------------------------------- */
    printf("Digite a data (DD/MM/AAAA): ");
    scanf("%10s", nova.data);
    getchar();

    /* --------------------------------------------------
     * Etapa 4 — Horário
     * -------------------------------------------------- */
    printf("Digite o horario (numero inteiro, ex: 9 para 09h00): ");
    scanf("%d", &nova.horario);
    getchar();

    /* --------------------------------------------------
     * Valor da consulta
     * -------------------------------------------------- */
    printf("Valor da consulta (R$): ");
    scanf("%f", &nova.valor);
    getchar();

    /* --------------------------------------------------
     * Etapa 6 — Verificar Conflito
     * VerificarConflito() compara data, horario e veterinario.id
     * -------------------------------------------------- */
    if (VerificarConflito(&lista, nova)) {
        printf("\n  O veterinario ja possui uma consulta marcada neste horario.\n");
        printf("  Agendamento cancelado.\n");
        return;
    }

    /* --------------------------------------------------
     * Etapas 7 e 8 — Inserir e Definir Status
     * -------------------------------------------------- */
    nova.id     = proximo_id;
    nova.status = AGENDADA;

    if (InserirConsulta(&lista, nova)) {
        proximo_id++;
        printf("\n  Consulta agendada com sucesso! (ID: %d)\n", nova.id);
    } else {
        printf("  Erro ao agendar consulta. Tente novamente.\n");
    }
}

/* =========================================================
 * Opção 2 — Realizar Consulta
 * ========================================================= */
static void realizarConsulta(void) {
    NoConsulta *atual;
    int         total_agendadas = 0;
    int         id;
    Consulta   *c;

    printf("\n=============================\n");
    printf("      REALIZAR CONSULTA\n");
    printf("=============================\n");

    /* Lista apenas consultas com status AGENDADA */
    printf("\nConsultas disponiveis (AGENDADA):\n");
    imprimirCabecalhoConsultas();

    atual = lista.inicio;
    while (atual != NULL) {
        if (atual->dados.status == AGENDADA) {
            imprimirLinhaConsulta(&atual->dados);
            total_agendadas++;
        }
        atual = atual->proximo;
    }

    if (total_agendadas == 0) {
        printf("  Nenhuma consulta agendada no momento.\n");
        return;
    }

    /* Solicita ID da consulta a realizar */
    printf("\nInforme o ID da consulta: ");
    scanf("%d", &id);
    getchar();

    c = buscarPorId(id);
    if (c == NULL) {
        printf("  Consulta nao encontrada.\n");
        return;
    }
    if (c->status != AGENDADA) {
        printf("  Esta consulta nao esta com status AGENDADA.\n");
        return;
    }

    /* Atualiza para EM ATENDIMENTO */
    c->status = EM_ATENDIMENTO;
    printf("\n  Status atualizado: EM ATENDIMENTO.\n");

    /* --------------------------------------------------
     * Execução clínica
     * ReallizarConsulta() coleta o estado geral do animal (0-10)
     * e atualiza o campo animal.estado (BOA/ESTAVEL/GRAVE/CRITICA).
     * -------------------------------------------------- */
    printf("\n  --- Dados Clinicos ---\n");
    RealizarConsulta(c);

    /* Registra e finaliza */
    RegistrarConsulta(*c);
    c->status = FINALIZADA;

    printf("\n  Consulta realizada com sucesso!\n");
    printf("  Status final: FINALIZADA.\n");
}

/* =========================================================
 * Opção 3 — Cancelar Consulta
 * ========================================================= */
static void cancelarConsulta(void) {
    NoConsulta *atual;
    int         total = 0;
    int         id;
    Consulta   *c;

    printf("\n=============================\n");
    printf("      CANCELAR CONSULTA\n");
    printf("=============================\n");

    /* Lista apenas AGENDADAS */
    printf("\nConsultas agendadas:\n");
    imprimirCabecalhoConsultas();

    atual = lista.inicio;
    while (atual != NULL) {
        if (atual->dados.status == AGENDADA) {
            imprimirLinhaConsulta(&atual->dados);
            total++;
        }
        atual = atual->proximo;
    }

    if (total == 0) {
        printf("  Nenhuma consulta agendada para cancelar.\n");
        return;
    }

    printf("\nInforme o ID da consulta a cancelar: ");
    scanf("%d", &id);
    getchar();

    c = buscarPorId(id);
    if (c == NULL) {
        printf("  Consulta nao encontrada.\n");
        return;
    }
    if (c->status != AGENDADA) {
        printf("  Apenas consultas AGENDADAS podem ser canceladas por aqui.\n");
        printf("  Use 'Atualizar Status' para alterar outros estados.\n");
        return;
    }

    /* Confirmação */
    printf("\n  Pet      : %s\n", c->animal.nome);
    printf("  Vet      : %s\n", c->veterinario.nome);
    printf("  Data     : %s\n", c->data);
    printf("  Horario  : %dh\n", c->horario);

    printf("\nDeseja realmente cancelar?\n");
    printf("1 - Sim\n");
    printf("2 - Nao\n");
    int conf = getOpcao();

    if (conf == 1) {
        int indice = buscarIndicePorId(id);
        if (RemoverConsulta(&lista, indice)) {
            printf("\n  Consulta cancelada.\n");
        } else {
            printf("  Erro ao cancelar a consulta.\n");
        }
    } else {
        printf("  Operacao cancelada.\n");
    }
}

/* =========================================================
 * Opção 4 — Atualizar Status
 * ========================================================= */
static void atualizarStatus(void) {
    NoConsulta *atual;
    int         id;
    Consulta   *c;
    int         opcao;

    printf("\n=============================\n");
    printf("      ATUALIZAR STATUS\n");
    printf("=============================\n");

    if (lista.tamanho == 0) {
        printf("  Nenhuma consulta cadastrada.\n");
        return;
    }

    /* Exibe todas as consultas */
    printf("\nConsultas:\n");
    imprimirCabecalhoConsultas();

    atual = lista.inicio;
    while (atual != NULL) {
        imprimirLinhaConsulta(&atual->dados);
        atual = atual->proximo;
    }

    printf("\nInforme o ID da consulta: ");
    scanf("%d", &id);
    getchar();

    c = buscarPorId(id);
    if (c == NULL) {
        printf("  Consulta nao encontrada.\n");
        return;
    }

    printf("\nNovo Status:\n");
    printf("1 - AGENDADA\n");
    printf("2 - EM ATENDIMENTO\n");
    printf("3 - FINALIZADA\n");
    printf("4 - CANCELADA\n");
    opcao = getOpcao();

    switch (opcao) {
        case 1: c->status = AGENDADA;       break;
        case 2: c->status = EM_ATENDIMENTO; break;
        case 3: c->status = FINALIZADA;     break;
        case 4: c->status = CANCELADA;      break;
        default:
            printf("  Opcao invalida.\n");
            return;
    }

    printf("\n  Status da consulta ID %d atualizado para: %s\n",
           c->id, statusParaString(c->status));
}

/* =========================================================
 * Opção 5 — Listar Consultas (ordenadas por Data e Horário)
 *
 * LP2 — Vetor Dinâmico:
 *   Aloca dinamicamente um vetor de ponteiros Consulta** para
 *   ordenar os registros sem mover os nós da lista encadeada.
 * ========================================================= */
static void listarConsultas(void) {
    int        i, j;
    Consulta **arr;
    NoConsulta *atual;

    printf("\n=============================\n");
    printf("       LISTAR CONSULTAS\n");
    printf("=============================\n");

    if (lista.tamanho == 0) {
        printf("  Nenhuma consulta cadastrada.\n");
        return;
    }

    /* Aloca vetor dinâmico de ponteiros — LP2: Vetor Dinâmico */
    arr = (Consulta **)malloc(lista.tamanho * sizeof(Consulta *));
    if (arr == NULL) {
        printf("  Erro de alocacao de memoria.\n");
        return;
    }

    /* Preenche o vetor com ponteiros para os dados de cada nó */
    atual = lista.inicio;
    for (i = 0; i < lista.tamanho; i++) {
        arr[i] = &(atual->dados);
        atual  = atual->proximo;
    }

    /* Ordenação por data, depois por horário — Selection Sort */
    for (i = 0; i < lista.tamanho - 1; i++) {
        int min = i;
        for (j = i + 1; j < lista.tamanho; j++) {
            int cmp = compararData(arr[j]->data, arr[min]->data);
            if (cmp < 0 || (cmp == 0 && arr[j]->horario < arr[min]->horario)) {
                min = j;
            }
        }
        if (min != i) {
            Consulta *tmp = arr[i];
            arr[i]        = arr[min];
            arr[min]      = tmp;
        }
    }

    /* Exibe o resultado ordenado */
    printf("\n");
    imprimirCabecalhoConsultas();
    for (i = 0; i < lista.tamanho; i++) {
        imprimirLinhaConsulta(arr[i]);
    }
    printf("\n  Total: %d consulta(s).\n", lista.tamanho);

    /* Libera o vetor dinâmico (a lista encadeada permanece intacta) */
    free(arr);
}

/* =========================================================
 * Impressão do menu de consultas
 * ========================================================= */
static void imprimirMenuConsultas(void) {
    printf("\n=============================\n");
    printf("          CONSULTAS\n");
    printf("=============================\n");
    printf("1 - Agendar Consulta\n");
    printf("2 - Realizar Consulta\n");
    printf("3 - Cancelar Consulta\n");
    printf("4 - Atualizar Status\n");
    printf("5 - Listar Consultas\n");
    printf("6 - Voltar\n");
}

/* =========================================================
 * MenuConsultas — ponto de entrada público
 * ========================================================= */
void MenuConsultas(void) {
    int opcao = 0;

    garantirInicializacao();

    while (opcao != 6) {
        imprimirMenuConsultas();

        opcao = getOpcao();

        switch (opcao) {
            case 1: agendarConsulta();  break;
            case 2: realizarConsulta(); break;
            case 3: cancelarConsulta(); break;
            case 4: atualizarStatus();  break;
            case 5: listarConsultas();  break;
            case 6:                     break;
            default:
                printf("\n  Opcao invalida.\n");
        }
    }
}
