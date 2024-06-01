#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define PORTA 8080
#define MAX_PRODUTOS 10
#define TAMANHO_BUFFER 1024
#define ARQUIVO_LOG "log.txt"
#define ARQUIVO_ESTOQUE "estoque.txt"
#define CPF_PROPRIETARIO "12345678900"

typedef struct {
    int id;
    char nome[50];
    float preco;
    int quantidade;
} Produto;

Produto produtos[MAX_PRODUTOS];
int num_produtos = 0;
int numero_pedido = 1;

void inicializar_produtos() {
    FILE *arquivo = fopen(ARQUIVO_ESTOQUE, "r");
    if (arquivo == NULL) {
        perror("Falha ao abrir o arquivo de estoque");
        exit(EXIT_FAILURE);
    }

    while (fscanf(arquivo, "%d %s %f %d", &produtos[num_produtos].id, produtos[num_produtos].nome, &produtos[num_produtos].preco, &produtos[num_produtos].quantidade) == 4) {
        num_produtos++;
        if (num_produtos >= MAX_PRODUTOS) break;
    }

    fclose(arquivo);
}

void atualizar_arquivo_estoque() {
    FILE *arquivo = fopen(ARQUIVO_ESTOQUE, "w");
    if (arquivo == NULL) {
        perror("Falha ao abrir o arquivo de estoque para escrita");
        return;
    }

    for (int i = 0; i < num_produtos; i++) {
        fprintf(arquivo, "%d %s %.2f %d\n", produtos[i].id, produtos[i].nome, produtos[i].preco, produtos[i].quantidade);
    }

    fclose(arquivo);
}

void registrar_compra(int id_pedido, const char* cpf, int id_produto, int quantidade) {
    FILE *arquivo_log = fopen(ARQUIVO_LOG, "a");
    if (arquivo_log == NULL) {
        perror("Falha ao abrir o arquivo de log");
        return;
    }

    time_t agora;
    time(&agora);
    char *timestamp = ctime(&agora);
    timestamp[strlen(timestamp) - 1] = '\0';

    fprintf(arquivo_log, "Pedido ID: %d, CPF: %s, Produto ID: %d, Nome: %s, Quantidade: %d, Timestamp: %s\n",
            id_pedido, cpf, produtos[id_produto - 1].id, produtos[id_produto - 1].nome, quantidade, timestamp);

    fclose(arquivo_log);
}

void registrar_falha_compra(const char* cpf, int id_produto, int quantidade_solicitada, int quantidade_disponivel) {
    FILE *arquivo_log = fopen(ARQUIVO_LOG, "a");
    if (arquivo_log == NULL) {
        perror("Falha ao abrir o arquivo de log");
        return;
    }

    time_t agora;
    time(&agora);
    char *timestamp = ctime(&agora);
    timestamp[strlen(timestamp) - 1] = '\0';

    fprintf(arquivo_log, "Tentativa de compra falhou, CPF: %s, Produto ID: %d, Quantidade solicitada: %d, Quantidade disponível: %d, Timestamp: %s\n",
            cpf, id_produto, quantidade_solicitada, quantidade_disponivel, timestamp);

    fclose(arquivo_log);
}

void obter_log_compra(int novo_socket, const char* cpf, int id_pedido) {
    if (strcmp(cpf, CPF_PROPRIETARIO) != 0) {
        char *resposta = "Permissão negada\n";
        send(novo_socket, resposta, strlen(resposta), 0);
        return;
    }

    FILE *arquivo_log = fopen(ARQUIVO_LOG, "r");
    if (arquivo_log == NULL) {
        perror("Falha ao abrir o arquivo de log");
        return;
    }

    char linha[TAMANHO_BUFFER];
    char resposta[TAMANHO_BUFFER * MAX_PRODUTOS] = "";
    int encontrado = 0;

    while (fgets(linha, sizeof(linha), arquivo_log) != NULL) {
        int id_pedido_registrado;
        sscanf(linha, "Pedido ID: %d,", &id_pedido_registrado);
        if (id_pedido_registrado == id_pedido) {
            strcat(resposta, linha);
            encontrado = 1;
        }
    }

    fclose(arquivo_log);

    if (encontrado) {
        strcat(resposta, "\n");
    } else {
        strcpy(resposta, "Pedido não encontrado\n");
    }

    send(novo_socket, resposta, strlen(resposta), 0);
}

void *tratar_cliente(void *socket_desc) {
    int novo_socket = *(int*)socket_desc;
    free(socket_desc);

    char buffer[TAMANHO_BUFFER] = {0};
    int leitura = read(novo_socket, buffer, TAMANHO_BUFFER);
    if (leitura > 0) {
        printf("Solicitação recebida: %s\n", buffer);

        if (strncmp(buffer, "estoque", 7) == 0) {
            char resposta[TAMANHO_BUFFER] = "";
            for (int i = 0; i < num_produtos; i++) {
                char info_produto[100];
                sprintf(info_produto, "ID: %d, Nome: %s, Preço: %.2f, Quantidade: %d\n", produtos[i].id, produtos[i].nome, produtos[i].preco, produtos[i].quantidade);
                strcat(resposta, info_produto);
            }
            strcat(resposta, "\n");
            send(novo_socket, resposta, strlen(resposta), 0);
        } else if (strncmp(buffer, "comprar", 7) == 0) {
            char cpf[12];
            int id_produto, quantidade;
            char *token = strtok(buffer + 8, " ");
            sscanf(token, "%s", cpf);
            token = strtok(NULL, " ");

            int numero_pedido_atual = numero_pedido++;
            int compra_valida = 1;

            int ids_produtos[MAX_PRODUTOS];
            int quantidades[MAX_PRODUTOS];
            int contagem = 0;

            while (token != NULL) {
                sscanf(token, "%d", &id_produto);
                token = strtok(NULL, " ");
                if (token == NULL) {
                    compra_valida = 0;
                    break;
                }
                sscanf(token, "%d", &quantidades[contagem]);
                ids_produtos[contagem] = id_produto;
                token = strtok(NULL, " ");
                contagem++;
            }

            if (!compra_valida) {
                char *resposta = "Formato de compra inválido\n";
                send(novo_socket, resposta, strlen(resposta), 0);
            } else {
                for (int i = 0; i < contagem; i++) {
                    if (ids_produtos[i] > 0 && ids_produtos[i] <= num_produtos) {
                        if (produtos[ids_produtos[i] - 1].quantidade < quantidades[i]) {
                            registrar_falha_compra(cpf, ids_produtos[i], quantidades[i], produtos[ids_produtos[i] - 1].quantidade);
                            compra_valida = 0;
                            break;
                        }
                    } else {
                        compra_valida = 0;
                        break;
                    }
                }

                if (compra_valida) {
                    for (int i = 0; i < contagem; i++) {
                        produtos[ids_produtos[i] - 1].quantidade -= quantidades[i];
                        registrar_compra(numero_pedido_atual, cpf, ids_produtos[i], quantidades[i]);
                    }
                    atualizar_arquivo_estoque();

                    char resposta[100];
                    sprintf(resposta, "Compra realizada com sucesso! Pedido ID: %d\n", numero_pedido_atual);
                    send(novo_socket, resposta, strlen(resposta), 0);
                } else {
                    char *resposta = "Estoque insuficiente ou ID de produto inválido\n";
                    send(novo_socket, resposta, strlen(resposta), 0);
                }
            }
        } else if (strncmp(buffer, "relatorio", 9) == 0) {
            char cpf[12];
            int id_pedido;
            sscanf(buffer + 10, "%s %d", cpf, &id_pedido);
            obter_log_compra(novo_socket, cpf, id_pedido);
        }
    } else {
        perror("Erro na leitura do socket");
    }
    close(novo_socket);
    return NULL;
}

int main() {
    int servidor_fd, novo_socket;
    struct sockaddr_in endereco;
    int tam_endereco = sizeof(endereco);

    inicializar_produtos();

    if ((servidor_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha ao criar o socket");
        exit(EXIT_FAILURE);
    }

    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = INADDR_ANY;
    endereco.sin_port = htons(PORTA);

    if (bind(servidor_fd, (struct sockaddr *)&endereco, sizeof(endereco)) < 0) {
        perror("Falha ao fazer bind do socket");
        close(servidor_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(servidor_fd, 5000) < 0) {
        perror("Falha ao escutar no socket");
        close(servidor_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor escutando na porta %d\n", PORTA);

    while (1) {
        if ((novo_socket = accept(servidor_fd, (struct sockaddr *)&endereco, (socklen_t*)&tam_endereco)) < 0) {
            perror("Falha ao aceitar a conexão");
            close(servidor_fd);
            exit(EXIT_FAILURE);
        }

        pthread_t thread_id;
        int *novo_sock = malloc(sizeof(int));
        *novo_sock = novo_socket;

        if (pthread_create(&thread_id, NULL, tratar_cliente, (void*) novo_sock) < 0) {
            perror("Falha ao criar a thread");
            close(novo_socket);
            free(novo_sock);
            continue;
        }

        pthread_detach(thread_id);
    }

    close(servidor_fd);
    return 0;
}
