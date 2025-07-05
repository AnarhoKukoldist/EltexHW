#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main (void) {
    int client_sock, port;
    struct sockaddr_in server;
    socklen_t server_len = sizeof(server);
    char buf[1024], c;

    if((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Ошибка: не удалось создать сокет.");
        exit(EXIT_FAILURE);
    }

    FILE* file = fopen("port.txt", "r");

    if (!file) {
        perror("Ошибка: не удалось открыть порт.");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", &port);
    fclose(file);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0) {
        perror("Ошибка: не удалось преобразовать IP адрес.");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    if (connect(client_sock, (struct sockaddr *)&server, server_len) < 0) {
        perror("Ошибка: не удалось установить соединение с сервером.");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Введите ваше сообщение: ");

        int i = 0;
        while ((c = getchar()) != '\n' && i < sizeof(buf)) {
            buf[i++] = c;
        }
        buf[i] = '\0';

        ssize_t send_len = send(client_sock, buf, sizeof(buf), 0);

        if (send_len < 0) {
            perror("Ошибка: не удалось отправить сообщение.");
            continue;
        }

        ssize_t recv_len = recv(client_sock, buf, sizeof(buf), 0);

        if (recv_len < 0) {
            perror("Ошибка: не удалось принять сообщение.");
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