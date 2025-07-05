#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

int main (void) {
    int client_sock, port;
    struct sockaddr_in server;
    socklen_t server_len = sizeof(server);
    char buf[1024], c;

    client_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (client_sock < 0) {
        perror("Ошибка: не удалось создать сокет.");
        exit(EXIT_FAILURE);
    }

    FILE* file = fopen("port.txt", "r");

    if (!file) {
        perror("Ошибка: не удалось открыть файл");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", &port);
    fclose(file);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) < 0) {
        perror("Ошибка: не удалось преобразовать IP адрес.");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Сообщение ваше сообщение: ");

        int i = 0;
        while ((c = getchar()) != '\n' && i < sizeof(buf)) {
            buf[i++] = c;
        }
        buf[i] = '\0';

        ssize_t send_len = sendto(client_sock, buf, sizeof(buf), 0, (struct sockaddr *)&server, server_len);

        if (send_len < 0) {
            perror("Не удалось отправить сообщение.");
            continue;
        }

        ssize_t recv_len = recvfrom(client_sock, buf, sizeof(buf), 0, (struct sockaddr *)&server, &server_len);

        if (recv_len < 0) {
            perror("Не удалось принять сообщение.");
            continue;
        }
        else if (recv_len == 0) {
            perror("Клиент отключился.");
            break;
        }

        printf("Сообщение от сервера: %s\n", buf);
    }

    close(client_sock);

    return 0;
}