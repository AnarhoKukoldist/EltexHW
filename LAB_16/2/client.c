#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>

#define SOCK_NAME "/tmp/server.sock"

int main (void) {
    int client_sock;
    struct sockaddr_un server;
	socklen_t server_len = sizeof(server);
    char buf[1024];
    char c;

    client_sock = socket(AF_LOCAL, SOCK_STREAM, 0);

    if (client_sock < 0) {
        perror("Ошибка: не удалось создать сокет.");
		exit(EXIT_FAILURE);
    }

    server.sun_family = AF_LOCAL;
    strcpy(server.sun_path, SOCK_NAME);

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