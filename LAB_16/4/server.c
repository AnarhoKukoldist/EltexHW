#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

int main (void) {
    srand(time(NULL));
    
    int server_sock, client_sock;
    struct sockaddr_in server, client;
    socklen_t server_len = sizeof(server);
    socklen_t client_len = sizeof(client);
    char buf[1024];

    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Ошибка: не удалось создать сокет.");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = 0;

    if (bind(server_sock, (const struct sockaddr *)&server, server_len) < 0) {
        perror("Ошибка: не удалось привязать порт к сокету.");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if (getsockname(server_sock, (struct sockaddr *)&server, &server_len) < 0) {
        perror("Ошибка: не удалось получить информацию о сокете.");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    FILE* file = fopen("port.txt", "w");

	if (!file) {
		perror("Ошибка: не удалось открыть файл.");
		close(server_sock);
		exit(EXIT_FAILURE);
	}

	fprintf(file, "%d", ntohs(server.sin_port));
	fclose(file);

    if (listen(server_sock, 5) < 0) {
        perror("Ошибка: не удалось принять запрос связи.");
		close(server_sock);
		exit(EXIT_FAILURE);
    }

    client_sock = accept(server_sock, (struct sockaddr *)&client, &client_len);

    if (client_sock < 0) {
        perror("Ошибка: не удалось установить соединение.");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        ssize_t recv_len = recv(client_sock, buf, sizeof(buf), 0);

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

        ssize_t send_len = send(client_sock, buf, sizeof(buf), 0);

        if (send_len < 0) {
            perror("Ошибка: не удалось отправить сообщение.");
            continue;
        }
    }

    close(server_sock);

    return 0;
}