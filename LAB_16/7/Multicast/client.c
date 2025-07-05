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
    broad_addr.sin_addr.s_addr = INADDR_ANY;
    broad_addr.sin_port = htons(7777);
    struct ip_mreqn mreqn;
    inet_pton(AF_INET, "224.0.0.1", &mreqn.imr_multiaddr.s_addr);
    mreqn.imr_address.s_addr = INADDR_ANY;
    mreqn.imr_ifindex = 0;
    setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreqn, sizeof(mreqn));
    bind(fd, (struct sockaddr *)&broad_addr, sizeof(broad_addr));
    
    char msg[10];
    struct sockaddr_in servaddr;
    socklen_t serv_len;
    recvfrom(fd, &msg, 10, 0, (struct sockaddr *) &servaddr, &serv_len);
    printf("msg from server - %s\n", msg);
    close(fd);
    return 0;
}
