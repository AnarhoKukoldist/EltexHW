#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define OWN_PORT 6666
#define SERVER_PORT 7777
#define SIZE 2048

int main (void) {
    int client_sock;
    char buf[SIZE];
    struct sockaddr_in server;
    struct iphdr* ip;
    struct udphdr* udp;

    client_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

    if (client_sock < 0) {
        perror("Ошибка: не удалось создать сокет");
		exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, "90.189.214.53", &server.sin_addr) <= 0) { // да, у меня есть белый ip
        perror("Ошибка: не удалось преобразовать IP адрес.");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    int i;
    char c;

    while (1) {
        i = sizeof(struct udphdr);

        printf("Введите ваше сообщение: ");
        while ((c = getchar()) != '\n' && (i < SIZE - sizeof(struct udphdr) - 2)) { // -2, чтобы учесть преобразование payload со стороны сервера
            buf[i++] = c;
        }
        buf[i] = '\0';
        
        udp = (struct udphdr *)buf;

        udp->source = htons(OWN_PORT);
        udp->dest = htons(SERVER_PORT);
        udp->len = htons(i);
        udp->check = 0;

        printf("msg: %s\n", buf + sizeof(struct udphdr));

        ssize_t send_len = sendto(client_sock, buf, i + 1, 0, (struct sockaddr *)&server, sizeof(server));

        if (send_len < 0) {
            perror("Ошибка: не удалось отправить сообщение");
            close(client_sock);
            exit(EXIT_FAILURE);
        }

        ssize_t recv_len = recvfrom(client_sock, buf, SIZE, 0, NULL, NULL);

        if (recv_len < 0) {
            perror("Ошибка: не удалось принять сообщение");
            close(client_sock);
            exit(EXIT_FAILURE);
        }

        ip = (struct iphdr *)buf;
        udp = (struct udphdr *)(buf + ip->ihl * 4);

        if (udp->dest == htons(OWN_PORT)) {
            printf("Сообщение от сервера: %s\n", buf + ip->ihl * 4 + sizeof(struct udphdr));
        }
    }

    close(client_sock);
    return 0;
}