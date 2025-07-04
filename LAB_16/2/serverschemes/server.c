#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

void get_current_time(char *buffer, size_t size) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    snprintf(buffer, size, "%04d-%02d-%02d %02d:%02d:%02d",
             timeinfo->tm_year + 1900,
             timeinfo->tm_mon + 1,
             timeinfo->tm_mday,
             timeinfo->tm_hour,
             timeinfo->tm_min,
             timeinfo->tm_sec);
}

int main() {
    int tcp_sock, udp_sock, max_fd;
    struct sockaddr_in addr;
    fd_set read_fds;
    
    // Создание TCP сокета
    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock < 0) {
        perror("TCP socket failed");
        exit(EXIT_FAILURE);
    }

    // Создание UDP сокета
    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock < 0) {
        perror("UDP socket failed");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    // Привязка сокетов
    if (bind(tcp_sock, (struct sockaddr*)&addr, sizeof(addr))) {
        perror("TCP bind failed");
        exit(EXIT_FAILURE);
    }

    if (bind(udp_sock, (struct sockaddr*)&addr, sizeof(addr))) {
        perror("UDP bind failed");
        exit(EXIT_FAILURE);
    }

    // Прослушивание TCP
    if (listen(tcp_sock, MAX_CLIENTS)) {
        perror("TCP listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d (TCP/UDP)...\n", PORT);

    int client_sockets[MAX_CLIENTS] = {0};
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(tcp_sock, &read_fds);
        FD_SET(udp_sock, &read_fds);
        max_fd = (tcp_sock > udp_sock) ? tcp_sock : udp_sock;

        // Добавление клиентских сокетов в набор
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &read_fds);
                if (client_sockets[i] > max_fd) {
                    max_fd = client_sockets[i];
                }
            }
        }

        // Ожидание активности
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select error");
            exit(EXIT_FAILURE);
        }

        // Обработка новых TCP подключений
        if (FD_ISSET(tcp_sock, &read_fds)) {
            int new_sock = accept(tcp_sock, (struct sockaddr*)&client_addr, &addr_len);
            if (new_sock < 0) {
                perror("accept failed");
                continue;
            }

            printf("New TCP connection: %s:%d\n", 
                   inet_ntoa(client_addr.sin_addr), 
                   ntohs(client_addr.sin_port));

            // Добавление нового сокета в массив
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_sock;
                    break;
                }
            }
        }

        // Обработка UDP запросов
        if (FD_ISSET(udp_sock, &read_fds)) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t len = recvfrom(udp_sock, buffer, BUFFER_SIZE, 0,
                                  (struct sockaddr*)&client_addr, &addr_len);
            
            if (len > 0) {
                printf("UDP request from %s:%d\n", 
                       inet_ntoa(client_addr.sin_addr), 
                       ntohs(client_addr.sin_port));
                
                char time_str[30];
                get_current_time(time_str, sizeof(time_str));
                sendto(udp_sock, time_str, strlen(time_str), 0,
                      (struct sockaddr*)&client_addr, addr_len);
            }
        }

        // Обработка TCP клиентов
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sock = client_sockets[i];
            if (sock > 0 && FD_ISSET(sock, &read_fds)) {
                memset(buffer, 0, BUFFER_SIZE);
                ssize_t len = recv(sock, buffer, BUFFER_SIZE, 0);
                
                if (len <= 0) {
                    // Закрытие соединения
                    getpeername(sock, (struct sockaddr*)&client_addr, &addr_len);
                    printf("TCP client disconnected: %s:%d\n",
                           inet_ntoa(client_addr.sin_addr),
                           ntohs(client_addr.sin_port));
                    close(sock);
                    client_sockets[i] = 0;
                } else {
                    // Отправка времени
                    printf("TCP request from %s:%d\n", 
                           inet_ntoa(client_addr.sin_addr), 
                           ntohs(client_addr.sin_port));
                    
                    char time_str[30];
                    get_current_time(time_str, sizeof(time_str));
                    send(sock, time_str, strlen(time_str), 0);
                }
            }
        }
    }

    close(tcp_sock);
    close(udp_sock);
    return 0;
}
