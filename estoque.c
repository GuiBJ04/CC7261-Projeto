#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CPF_CORRETO "12345678900"
#define TAM_MAX 100

typedef struct {
	int id;
	char nome[50];
	float preco;
	int quantidade;
} Produto;

Produto estoque[TAM_MAX];
int num_produtos = 0;

void carregar_estoque() {
	FILE *arquivo = fopen("estoque.txt", "r");
	if (arquivo == NULL) {
    	printf("Erro ao abrir o arquivo de estoque.\n");
    	exit(1);
	}

	while (fscanf(arquivo, "%d %s %f %d", &estoque[num_produtos].id, estoque[num_produtos].nome, &estoque[num_produtos].preco, &estoque[num_produtos].quantidade) != EOF) {
    	num_produtos++;
	}
	fclose(arquivo);
}

void salvar_estoque() {
	FILE *arquivo = fopen("estoque.txt", "w");
	if (arquivo == NULL) {
    	printf("Erro ao abrir o arquivo de estoque.\n");
    	exit(1);
	}

	for (int i = 0; i < num_produtos; i++) {
    	fprintf(arquivo, "%d %s %.2f %d\n", estoque[i].id, estoque[i].nome, estoque[i].preco, estoque[i].quantidade);
	}
	fclose(arquivo);
}

int obter_novo_id() {
	int maior_id = 0;
	for (int i = 0; i < num_produtos; i++) {
    	if (estoque[i].id > maior_id) {
        	maior_id = estoque[i].id;
    	}
	}
	return maior_id + 1;
}

void adicionar_produto() {
	if (num_produtos >= TAM_MAX) {
    	printf("Estoque cheio.\n");
    	return;
	}

	Produto novo;
	novo.id = obter_novo_id();
	int temp = novo.id;
	printf("Digite o nome do produto: ");
	scanf("%s", novo.nome);
	printf("Digite o preco do produto: ");
	scanf("%f", &novo.preco);
	printf("Digite a quantidade do produto: ");
	scanf("%d", &novo.quantidade);

	estoque[num_produtos] = novo;
	num_produtos++;
	salvar_estoque();
	printf("Produto adicionado com sucesso.\n");
	printf("Id do produto: %d\n", temp);
}

void remover_produto() {
	int id;
	printf("Digite o ID do produto a ser removido: ");
	scanf("%d", &id);

	int encontrado = 0;
	for (int i = 0; i < num_produtos; i++) {
    	if (estoque[i].id == id) {
        	encontrado = 1;
        	for (int j = i; j < num_produtos - 1; j++) {
            	estoque[j] = estoque[j + 1];
        	}
        	num_produtos--;
        	salvar_estoque();
        	printf("Produto %d removido com sucesso.\n", id);
        	break;
    	}
	}
	if (!encontrado) {
    	printf("Produto nao encontrado.\n");
	}
}

void atualizar_quantidade() {
	int id, quantidade;
	printf("Digite o ID do produto: ");
	scanf("%d", &id);
	printf("Digite a nova quantidade: ");
	scanf("%d", &quantidade);

	int encontrado = 0;
	for (int i = 0; i < num_produtos; i++) {
    	if (estoque[i].id == id) {
        	estoque[i].quantidade = quantidade;
        	salvar_estoque();
        	printf("Quantidade atualizada com sucesso.\n");
        	encontrado = 1;
        	break;
    	}
	}
	if (!encontrado) {
    	printf("Produto nao encontrado.\n");
	}
}

int verificar_cpf() {
	char cpf[12];
	printf("Digite seu CPF: ");
	scanf("%s", cpf);
	return strcmp(cpf, CPF_CORRETO) == 0;
}

int main() {
	int opcao;

	carregar_estoque();

	if (!verificar_cpf()) {
    	printf("CPF incorreto. Acesso negado.\n");
    	return 1;
	}

	do {
    	printf("\n1. Adicionar produto\n");
    	printf("2. Remover produto\n");
    	printf("3. Atualizar quantidade\n");
    	printf("4. Sair\n");
    	printf("Escolha uma opcao: ");
    	scanf("%d", &opcao);

    	switch (opcao) {
        	case 1:
            	adicionar_produto();
            	break;
        	case 2:
            	remover_produto();
            	break;
        	case 3:
            	atualizar_quantidade();
            	break;
        	case 4:
            	printf("Saindo...\n");
            	break;
        	default:
            	printf("Opcao invalida.\n");
    	}
	} while (opcao != 4);

	return 0;
}
