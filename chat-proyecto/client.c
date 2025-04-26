#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 2048

int sockfd;
char username[32];

void *receive_handler(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int len = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (len > 0) {
            buffer[len] = '\0';
            printf("%s\n", buffer);
        } else {
            break;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;
    pthread_t recv_thread;

    if (argc != 3) {
        printf("Uso: %s <IP del servidor> <Puerto>\n", argv[0]);
        return 1;
    }

    printf("Ingresa tu nombre: ");
    fgets(username, 32, stdin);
    username[strcspn(username, "\n")] = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Conexión fallida");
        return 1;
    }

    send(sockfd, username, 32, 0);
    pthread_create(&recv_thread, NULL, receive_handler, NULL);

    char buffer[BUFFER_SIZE];
    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        send(sockfd, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "quit") == 0) {
            break;
        }
    }

    close(sockfd);
    return 0;
}
