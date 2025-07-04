#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define SERVER_PORT 7777
#define SIZE 2048

struct client_data {
    char ip[16];
    unsigned int port;
    unsigned int counter;
};

void create (struct client_data** client_data) {
    *client_data = (struct client_data *)malloc(10 * sizeof(struct client_data));

    if (!client_data) {
        perror("Ошибка: не удалось выделить память под пользовательскую структуру");
        exit(EXIT_FAILURE);
    }
}

void increase (struct client_data** client_data, unsigned int size) {
    *client_data = (struct client_data *)realloc(*client_data, size * sizeof(struct client_data));

    if (!client_data) {
        perror("Ошибка: не удалось изменить размер пользовательской структуры");
        exit(EXIT_FAILURE);
    }
}

int main (void) {
    int server_sock, size = 0;
    char buf_recv[SIZE], buf_send[SIZE];
    struct sockaddr_in client;
    struct client_data* client_data;
    struct iphdr* ip;
    struct udphdr* udp;
    struct in_addr src;

    server_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

    if (server_sock < 0) {
        perror("Ошибка: не удалось создать сокет");
		exit(EXIT_FAILURE);
    }

    while (1) {
        ssize_t recv_len = recvfrom(server_sock, buf_recv, sizeof(buf_recv), 0, NULL, NULL);

        if (recv_len < 0) {
            perror("Ошибка: не удалось принять пакет");
			continue;
        }

        ip = (struct iphdr *)buf_recv;
        udp = (struct udphdr *)(buf_recv + ip->ihl * 4); // первые 20 байт - ip заголовок
        src.s_addr = ip->saddr;

        //printf("1\n");
        //printf("Сообщение от клиента %s:%d -> %s\n", inet_ntoa(src), ntohs(udp->dest), buf_recv + ip->ihl * 5 + sizeof(struct udphdr));

        if (udp->dest == htons(SERVER_PORT)) {
            src.s_addr = ip->saddr;

            printf("Сообщение от клиента %s:%d -> %s\n", inet_ntoa(src), ntohs(udp->source), buf_recv + ip->ihl * 4 + sizeof(struct udphdr));

            if (!size) {
                create(&client_data);
            }
            else if (size % 10 == 0) { // когда количество записей становится кратно 10, тогда увеличивает массив ещё на 10
                increase(&client_data, size + 10);
            }

            int temp;
            for (temp = 0; temp < size; temp++) {
                if (!strcmp(client_data[temp].ip, inet_ntoa(src))) {
                    if (client_data[temp].port == ntohs(udp->dest)) { // если такой клиент уже был, то увеличиваем счётчик
                        client_data[temp].counter++;
                        break;
                    }
                }
            }

            if (temp == size) { // не было ни одного вхождения в массив с такими адресом и портом
                strcpy(client_data[temp].ip, inet_ntoa(src));
                client_data[temp].port = ntohs(udp->dest);
                client_data[temp].counter = 1;
                size++;
            }

            strcpy(buf_send + sizeof(struct udphdr), buf_recv + ip->ihl * 4 + sizeof(struct udphdr));

            int i = ntohs(ip->tot_len) - ip->ihl * 4; // конец payload

            buf_send[i - 1] = ' ';
            buf_send[i] = client_data[temp].counter  + '0';
            buf_send[i + 1] = '\0';

            struct udphdr* udp_send = (struct udphdr *)buf_send;

            udp_send->dest = udp->source;
            udp_send->source = udp->dest;
            udp_send->check = 0;
            udp_send->len = htons(i + 1); // чтобы учесть изменения в payload

            client.sin_family = AF_INET;
            client.sin_port = udp_send->dest;
            client.sin_addr.s_addr = ip->saddr;

            sendto(server_sock, buf_send, i + 1, 0, (struct sockaddr *)&client, sizeof(client));
        }
    }

    close(server_sock);
    return 0;
}
