#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/udp.h>

#define OWN_PORT 7777

int main (void) {
    int client_sock, server_port;
    struct sockaddr_in server;
    char buf[1024];
    struct udphdr* udp = (struct udphdr *)buf;

    client_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

    if (client_sock < 0) {
        perror("Ошибка: не удалось создать сокет");
		exit(EXIT_FAILURE);
    }

    FILE* file = fopen("port.txt", "r");

    if (!file) {
        perror("Ошибка: не удалось открыть файл");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", &server_port);

    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);

    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0) {
        perror("Ошибка: не удалось преобразовать IP адрес.");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    printf("Введите ваше сообщение: ");

    int i = sizeof(struct udphdr);
    char c;
    while ((c = getchar()) != '\n' && i < sizeof(buf)) {
        buf[i++] = c;
    }
    buf[i++] = '\0';
    
    udp->source = htons(OWN_PORT);
    udp->dest = htons(server_port);
    udp->len = htons(i);
    udp->check = 0;

    size_t total_len = i;

    ssize_t send_len = sendto(client_sock, buf, total_len, 0, (struct sockaddr *)&server, sizeof(server));

    if (send_len < 0) {
        perror("Ошибка: не удалось отправить сообщение");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        ssize_t recv_len = recvfrom(client_sock, buf, sizeof(buf), 0, NULL, NULL);

        if (recv_len < 0) {
			perror("Ошибка: не удалось принять пакет");
			continue;
		}

        udp = (struct udphdr *)(buf + 20); // 20 первых байт под ip заголовок

        if (udp->dest == htons(OWN_PORT)) {
            printf("Сообщение от сервера: %s\n", buf + 20 + sizeof(struct udphdr));
        }
        else {
            continue;
        }
    }

    close(client_sock);

    return 0;
}