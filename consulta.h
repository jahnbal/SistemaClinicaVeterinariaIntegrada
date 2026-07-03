#ifndef CONSULTA_H
#define CONSULTA_H

#include "funcionario.h"
#include "pets.h"

// Status da consulta
typedef enum {
    AGENDADA,
    EM_ATENDIMENTO,
    FINALIZADA,
    CANCELADA
} StatusConsulta;


//Estrutura principal de uma consulta
typedef struct {
    int            id;           /* identificador único gerado em sequência   */
    Pet            animal;       /* pet em atendimento                        */
    Funcionario    veterinario;  /* veterinário responsável                   */
    float          valor;        /* valor cobrado pela consulta               */
    char           data[11];     /* formato DD/MM/AAAA                        */
    int            horario;      /* hora inteira, ex: 9 = 09h00, 14 = 14h00  */
    StatusConsulta status;       /* estado atual da consulta                  */
} Consulta;


 // Nó da lista encadeada — LP2: Lista Encadeada

typedef struct NoConsulta {
    Consulta          dados;
    struct NoConsulta *proximo;
} NoConsulta;


 // Cabeça da lista encadeada

typedef struct {
    NoConsulta *inicio;
    int         tamanho;
} ListaConsulta;


 // Protótipos — funções em consulta.c

Consulta  *MarcarConsulta(Consulta *consulta);
int        RealizarConsulta(Consulta *consulta);
int        RegistrarConsulta(Consulta consulta);
void       InicializarLista(ListaConsulta *lista);
int        VerificarConflito(ListaConsulta *lista, Consulta consulta);
int        InserirConsulta(ListaConsulta *lista, Consulta consulta);
int        RemoverConsulta(ListaConsulta *lista, int indice);
Consulta  *BuscarConsulta(ListaConsulta *lista, int indice);
void       LiberarLista(ListaConsulta *lista);

#endif /* CONSULTA_H */
