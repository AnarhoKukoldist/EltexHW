#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define N 10

int main(){

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }
    int broadcastenable = 1;
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcastenable, sizeof(broadcastenable));
    struct sockaddr_in broad_addr;
    broad_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "224.0.0.1", &broad_addr.sin_addr);
    broad_addr.sin_port = htons(7777);
    char msg[N] = "Hi!!";
    while(1){
        if (sendto(fd, &msg, N, 0, (struct sockaddr *) &broad_addr, sizeof(broad_addr)) == -1){
            perror("sendto");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}
