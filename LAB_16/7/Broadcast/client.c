#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

int main(){
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in broad_addr;
    broad_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "255.255.255.255", &broad_addr.sin_addr);
    broad_addr.sin_port = htons(7777);
    bind(fd, (struct sockaddr *)&broad_addr, sizeof(broad_addr));
    char msg[10];
    struct sockaddr_in servaddr;
    socklen_t serv_len;
    recvfrom(fd, &msg, 10, 0, (struct sockaddr *) &servaddr, &serv_len);
    printf("msg from server - %s\n", msg);
    close(fd);
    return 0;
}
