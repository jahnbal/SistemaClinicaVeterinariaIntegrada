#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menu_produto.h"
#include "../utils/utilidades.h"
#include "../produtos.h"

static void imprimirMenuProduto(void)
{
    printf("\n========================================\n");
    printf("          MENU PRODUTO\n");
    printf("========================================\n");

    printf("1 - Cadastrar Produto\n");
    printf("2 - Remover Produto\n");
    printf("3 - Atualizar Produto\n");
    printf("4 - Voltar\n");
}

static void limparBufferEntrada(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static int lerInteiroPositivo(const char *prompt)
{
    int valor;
    int c;

    for (;;) {
        printf("%s", prompt);
        if (scanf("%d", &valor) == 1 && valor > 0) {
            while ((c = getchar()) != '\n' && c != EOF);
            return valor;
        }
        printf("Entrada invalida. Digite um numero inteiro positivo.\n");
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

static int lerInteiroNaoNegativo(const char *prompt)
{
    int valor;
    int c;

    for (;;) {
        printf("%s", prompt);
        if (scanf("%d", &valor) == 1 && valor >= 0) {
            while ((c = getchar()) != '\n' && c != EOF);
            return valor;
        }
        printf("Entrada invalida. Digite um numero inteiro nao negativo.\n");
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

static float lerFloatNaoNegativo(const char *prompt)
{
    float valor;
    int c;

    for (;;) {
        printf("%s", prompt);
        if (scanf("%f", &valor) == 1 && valor >= 0.0f) {
            while ((c = getchar()) != '\n' && c != EOF);
            return valor;
        }
        printf("Entrada invalida. Digite um numero real nao negativo.\n");
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

static void lerTexto(const char *prompt, char *destino, size_t tamanho)
{
    if (destino == NULL || tamanho == 0)
        return;

    while (1) {
        printf("%s", prompt);
            if (fgets(destino, (int)tamanho, stdin) == NULL) {
            destino[0] = '\0';
            return;
        }

        size_t len = strcspn(destino, "\r\n");
        destino[len] = '\0';

        if (len > 0) {
            return;
        }

        printf("Entrada vazia. Tente novamente.\n");
    }
}

static void cadastrarProduto(void)
{
    Produto novoProduto;
    FILE *arqProdutos = fopen(ARQUIVO_PRODS, "rb+");

    if (arqProdutos == NULL) {
        arqProdutos = fopen(ARQUIVO_PRODS, "ab+");
        if (arqProdutos == NULL) {
            perror("Erro ao abrir o arquivo de produtos");
            return;
        }
    }

    memset(&novoProduto, 0, sizeof(Produto));

    printf("\n[Cadastrar Produto]\n");
    novoProduto.id = lerInteiroPositivo("ID do produto: ");
    lerTexto("Nome do produto: ", novoProduto.nome, sizeof(novoProduto.nome));
    novoProduto.preco = lerFloatNaoNegativo("Preco do produto: R$");
    novoProduto.quantidade = lerInteiroNaoNegativo("Quantidade em estoque: ");
    lerTexto("Descricao do produto: ", novoProduto.descricao,
             sizeof(novoProduto.descricao));
    novoProduto.ativo = 1;

    if (InsereProduto(&novoProduto, arqProdutos)) {
        printf("Produto cadastrado com sucesso.\n");
    } else {
        printf("Falha ao cadastrar o produto.\n");
    }

    fclose(arqProdutos);
}

static void removerProduto(void)
{
    Produto produtoRemover;
    FILE *arqProdutos = fopen(ARQUIVO_PRODS, "rb");

    if (arqProdutos == NULL) {
        printf("Nenhum produto cadastrado ou arquivo de produtos inacessivel.\n");
        return;
    }

    memset(&produtoRemover, 0, sizeof(Produto));

    printf("\n[Remover Produto]\n");
    produtoRemover.id = lerInteiroPositivo("ID do produto a remover: ");

    if (RemoveProduto(&produtoRemover, arqProdutos)) {
        printf("Produto com ID %d removido com sucesso.\n", produtoRemover.id);
    } else {
        printf("Falha ao remover o produto com ID %d.\n", produtoRemover.id);
    }
}

static void atualizarProduto(void)
{
    Produto produtoAtual;
    Produto produtoAlterado;
    FILE *arqProdutos = fopen(ARQUIVO_PRODS, "rb+");
    long pos = 0;
    int campo;

    if (arqProdutos == NULL) {
        printf("Nenhum produto cadastrado ou arquivo de produtos inacessivel.\n");
        return;
    }

    memset(&produtoAtual, 0, sizeof(Produto));
    memset(&produtoAlterado, 0, sizeof(Produto));

    printf("\n[Atualizar Produto]\n");
    produtoAtual.id = lerInteiroPositivo("ID do produto a atualizar: ");

    rewind(arqProdutos);
    while (fread(&produtoAlterado, sizeof(Produto), 1, arqProdutos) == 1) {
        if (produtoAlterado.id == produtoAtual.id && produtoAlterado.ativo) {
            break;
        }
        pos += sizeof(Produto);
    }

    if (produtoAlterado.id != produtoAtual.id || !produtoAlterado.ativo) {
        printf("Produto com ID %d nao encontrado.\n", produtoAtual.id);
        fclose(arqProdutos);
        return;
    }

    printf("Produto encontrado: %s | R$ %.2f | Estoque: %d\n",
           produtoAlterado.nome, produtoAlterado.preco,
           produtoAlterado.quantidade);
    printf("Descricao: %s\n", produtoAlterado.descricao);

    printf("\nCampos para atualizar:\n");
    printf("1 - Nome\n");
    printf("2 - Preco\n");
    printf("3 - Quantidade\n");
    printf("4 - Descricao\n");
    printf("0 - Cancelar\n");
    campo = getOpcao();

    switch (campo) {
        case 1:
            lerTexto("Novo nome: ", produtoAlterado.nome,
                     sizeof(produtoAlterado.nome));
            break;
        case 2:
            produtoAlterado.preco = lerFloatNaoNegativo("Novo preco: R$");
            break;
        case 3:
            produtoAlterado.quantidade = lerInteiroNaoNegativo(
                "Nova quantidade em estoque: ");
            break;
        case 4:
            lerTexto("Nova descricao: ", produtoAlterado.descricao,
                     sizeof(produtoAlterado.descricao));
            break;
        case 0:
            printf("Atualizacao cancelada.\n");
            fclose(arqProdutos);
            return;
        default:
            printf("Opcao invalida.\n");
            fclose(arqProdutos);
            return;
    }

    if (fseek(arqProdutos, pos, SEEK_SET) != 0) {
        perror("Erro ao posicionar arquivo para atualizar");
        fclose(arqProdutos);
        return;
    }

    if (fwrite(&produtoAlterado, sizeof(Produto), 1, arqProdutos) != 1) {
        printf("Erro ao gravar atualizacao do produto.\n");
        fclose(arqProdutos);
        return;
    }

    fflush(arqProdutos);
    printf("Produto ID %d atualizado com sucesso.\n", produtoAlterado.id);
    fclose(arqProdutos);
}

void MenuProduto(void)
{
    int opcao = 0;

    while(opcao != 4)
    {
        imprimirMenuProduto();

        opcao = getOpcao();

        switch(opcao)
        {
            case 1:
                cadastrarProduto();
                break;

            case 2:
                removerProduto();
                break;

            case 3:
                atualizarProduto();
                break;

            case 4:
                break;

            default:
                printf("\nOpcao invalida.\n");
        }
    }
}
