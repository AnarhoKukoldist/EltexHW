#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#define SERVER_PORT 7777

int main (void) {
    srand(time(NULL));

    int server_sock;
    struct sockaddr_in server, client;
    socklen_t server_len = sizeof(server);
    socklen_t client_len = sizeof(client);
    char buf[1024];

    server_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_sock < 0) {
        perror("Ошибка: не удалось создать сокет.");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(SERVER_PORT);

    if (bind(server_sock, (struct sockaddr *)&server, server_len) < 0) {
        perror("Ошибка: не удалось не удалось привязать порт к сокету.");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if (getsockname(server_sock, (struct sockaddr *)&server, &server_len) < 0) {
        perror("Ошибка: не удалось получить информацию о сокете");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        ssize_t recv_len = recvfrom(server_sock, buf, sizeof(buf), 0, (struct sockaddr *)&client, &client_len);

        if (recv_len < 0) {
            perror("Не удалось принять сообщение.");
            continue;
        }
        else if (recv_len == 0) {
            perror("Клиент отключился.");
            break;
        }

        printf("Сообщение от клиента: %s\n", buf);

        buf[0] = (char)(rand() % 127);

        ssize_t send_len = sendto(server_sock, buf, sizeof(buf), 0, (struct sockaddr *)&client, client_len);

        if (send_len < 0) {
            perror("Не удалось отправить сообщение.");
            continue;
        }
    }

    close(server_sock);

    return 0;
}