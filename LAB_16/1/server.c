#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <time.h>

#define SOCK_NAME "/tmp/server.sock"

int main (void) {
    srand(time(NULL));

    int server_sock;
    struct sockaddr_un server, client;
    socklen_t client_len = sizeof(client);
	socklen_t server_len = sizeof(server);
    char buf[1024];

    unlink(SOCK_NAME);

    server_sock = socket(AF_LOCAL, SOCK_DGRAM, 0);

    if (server_sock < 0) {
		perror("Ошибка: не удалось создать сокет.");
		exit(EXIT_FAILURE);
	}

    server.sun_family = AF_LOCAL;
    strcpy(server.sun_path, SOCK_NAME);

    if (bind(server_sock, (const struct sockaddr *)&server, server_len) < 0) {
        perror("Ошибка: не удалось привязать порт к сокету");
		close(server_sock);
		exit(EXIT_FAILURE);
    }

    while (1) {
        ssize_t recv_len = recvfrom(server_sock, buf, sizeof(buf), 0, (struct sockaddr *)&client, &client_len);

        if (recv_len < 0) {
            perror("Ошибка: не удалось принять сообщение.");
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
            perror("Ошибка: не удалось отправить сообщение.");
            continue;
        }
    }

    close(server_sock);
    unlink(SOCK_NAME);

    return 0;
}