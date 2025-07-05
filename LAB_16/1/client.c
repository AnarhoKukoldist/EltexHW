#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>

#define SERVER_SOCK_NAME "/tmp/server.sock"
#define CLIENT_SOCK_NAME "/tmp/client.sock"

int main (void) {
    int client_sock;
    struct sockaddr_un server, client;
    socklen_t client_len = sizeof(client);
	socklen_t server_len = sizeof(server);
    char buf[1024];
    char c;

    unlink(CLIENT_SOCK_NAME);

    client_sock = socket(AF_LOCAL, SOCK_DGRAM, 0);

    if (client_sock < 0) {
        perror("Ошибка: не удалось создать сокет.");
		exit(EXIT_FAILURE);
    }

    client.sun_family = AF_LOCAL;
    strcpy(client.sun_path, CLIENT_SOCK_NAME);

    if (bind(client_sock, (const struct sockaddr *)&client, client_len) < 0) {
        perror("Ошибка: не удалось привязать порт к сокету");
		close(client_sock);
		exit(EXIT_FAILURE);
    }

    server.sun_family = AF_LOCAL;
    strcpy(server.sun_path, SERVER_SOCK_NAME);

    while (1) {
        printf("Введите ваше сообщение: ");

        int i = 0;
        while ((c = getchar()) != '\n' && i < sizeof(buf)) {
            buf[i++] = c;
        }
        buf[i] = '\0';

        ssize_t send_len = sendto(client_sock, buf, sizeof(buf), 0, (struct sockaddr *)&server, server_len);

        if (send_len < 0) {
            perror("Ошибка: не удалось отправить сообщение.");
            continue;
        }

        ssize_t recv_len = recvfrom(client_sock, buf, sizeof(buf), 0, (struct sockaddr *)&server, &server_len);

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
    unlink(CLIENT_SOCK_NAME);

    return 0;
}