#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define SERVER_PORT 8888
#define MAX_CLIENTES 10

typedef struct {
    int sock;
    char username[30];
} Cliente;

Cliente clientes[MAX_CLIENTES];
int num_clientes = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void enviar_a_todos(char *mensaje, int excluido) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i].sock != excluido) {
            send(clientes[i].sock, mensaje, strlen(mensaje), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *manejar_cliente(void *arg) {
    Cliente cliente = *((Cliente *)arg);
    char mensaje[1024];
    int len;

    char bienvenida[128];
    sprintf(bienvenida, "Bienvenido al chat, %s!\n", cliente.username);
    send(cliente.sock, bienvenida, strlen(bienvenida), 0);

    while (1) {
        len = recv(cliente.sock, mensaje, sizeof(mensaje)-1, 0);
        if (len <= 0) {
            break;
        }
        mensaje[len] = '\0';

        if (strcmp(mensaje, "Desconectado") == 0) {
            break;
        }

        printf("%s: %s\n", cliente.username, mensaje);

        char mensaje_completo[1024];
        sprintf(mensaje_completo, "%s: %s", cliente.username, mensaje);
        enviar_a_todos(mensaje_completo, cliente.sock);
    }

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i].sock == cliente.sock) {
            for (int j = i; j < num_clientes - 1; j++) {
                clientes[j] = clientes[j + 1];
            }
            num_clientes--;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    close(cliente.sock);
    printf("Cliente %s desconectado.\n", cliente.username);
    return NULL;
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t hilo_cliente;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Error al crear el socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind");
        return 1;
    }

    if (listen(server_sock, 5) < 0) {
        perror("Error al escuchar");
        return 1;
    }

    printf("Servidor escuchando en puerto %d...\n", SERVER_PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Error al aceptar conexión");
            continue;
        }

        char username[30];
        recv(client_sock, username, sizeof(username), 0);

        if (num_clientes < MAX_CLIENTES) {
            clientes[num_clientes].sock = client_sock;
            strcpy(clientes[num_clientes].username, username);
            num_clientes++;

            pthread_create(&hilo_cliente, NULL, manejar_cliente, (void *)&clientes[num_clientes - 1]);
        } else {
            printf("Máximo de clientes alcanzado.\n");
            close(client_sock);
        }
    }

    close(server_sock);
    return 0;
}

