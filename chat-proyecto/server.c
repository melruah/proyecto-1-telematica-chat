#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048

typedef struct {
    struct sockaddr_in address;
    int socket;
    char username[32];
} client_t;

client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_message(const char *msg) {
    FILE *log_file = fopen("logs/chat.log", "a");
    if (log_file) {
        fprintf(log_file, "%s\n", msg);
        fclose(log_file);
    }
}

void send_message_to_all(char *message, int sender_socket) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->socket != sender_socket) {
            send(clients[i]->socket, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE + 32];
    int leave_flag = 0;

    client_t *cli = (client_t *)arg;

    // Nombre de usuario
    if (recv(cli->socket, cli->username, 32, 0) <= 0) {
        printf("No se recibió nombre de usuario.\n");
        leave_flag = 1;
    } else {
        sprintf(message, "%s se ha unido al chat", cli->username);
        printf("%s\n", message);
        log_message(message);
        send_message_to_all(message, cli->socket);
    }

    while (!leave_flag) {
        int len = recv(cli->socket, buffer, BUFFER_SIZE, 0);
        if (len > 0) {
            buffer[len] = '\0';
            if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "quit") == 0) {
                leave_flag = 1;
                sprintf(message, "%s ha salido del chat", cli->username);
                printf("%s\n", message);
                log_message(message);
                send_message_to_all(message, cli->socket);
            } else {
                sprintf(message, "%s: %s", cli->username, buffer);
                printf("%s\n", message);
                log_message(message);
                send_message_to_all(message, cli->socket);
            }
        } else {
            leave_flag = 1;
        }
    }

    close(cli->socket);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] == cli) {
            clients[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    free(cli);
    pthread_detach(pthread_self());

    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    pthread_t tid;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("No se pudo crear el socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Falló bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0) {
        perror("Falló listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto 8888...\n");

    while (1) {
        socklen_t clilen = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &clilen);

        if (client_socket >= 0) {
            client_t *cli = (client_t *)malloc(sizeof(client_t));
            cli->address = client_addr;
            cli->socket = client_socket;

            pthread_mutex_lock(&clients_mutex);
            int i;
            for (i = 0; i < MAX_CLIENTS; ++i) {
                if (!clients[i]) {
                    clients[i] = cli;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);

            if (i < MAX_CLIENTS) {
                pthread_create(&tid, NULL, handle_client, (void*)cli);
            } else {
                char *full_msg = "Servidor lleno. Intenta más tarde.";
                send(client_socket, full_msg, strlen(full_msg), 0);
                close(client_socket);
                free(cli);
            }
        }
    }

    return 0;
}
