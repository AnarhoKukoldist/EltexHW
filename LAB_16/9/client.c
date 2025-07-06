#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <sys/ioctl.h>  

#define OWN_PORT 6666
#define SERVER_PORT 7777
#define SIZE 2048

int checksum (char* buf) {
    int csum = 0;
    short* ptr = (short *)buf;

    for (int i = 0; i < 10; i++) {
        csum += *ptr;
        ptr++;
    }

    csum = (csum & 0xFFFF) + (csum >> 16);
    csum = ~csum;

    return csum;
}

int main (void) {
    int client_sock;
    char buf[SIZE];
    unsigned char dst_mac[6] = {0x70, 0x1a, 0xb8, 0x04, 0x91, 0xc9};
    struct sockaddr_ll server;
    struct ifreq ifr
    struct ethhdr* eth = (struct ethhdr *)buf;
    struct iphdr* ip = (struct iphdr *)(buf + sizeof(ethhdr));
    struct udphdr* udp = (struct udphdr *)(buf + sizeof(ethhdr) + 20);

    client_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (client_sock < 0) {
        perror("Ошибка: не удалось создать сокет");
		exit(EXIT_FAILURE);
    }

    int i = sizeof(ethhdr) + 20 + sizeof(struct udphdr);
    char c;
    while ((c = getchar()) != '\n' && i < SIZE) {
        buf[i++] = c;
    }
    buf[i] = '\0';

    strncpy(ifr.ifr_name, "wlo1", IFNAMESIZ - 1);
    ioctl(client_sock, SIOCGIFHWADDR, &ifr);
    memcpy(eth->h_source, ifr.ifr_hwaddr.sa_data, 6);
    memcpy(eth->h_dest, dst_mac, 6);
    eth->h_proto = htons(ETH_P_IP);

    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = htons(i - sizeof(struct ethhdr));
    ip->id = htons(0);
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_UDP;
    ip->check = 0;
    ip->saddr = inet_addr("192.168.0.3");
    ip->daddr = inet_addr("192.168.0.1");

    ip->check = checksum(buf + sizeof(struct ethhdr));

    udp->source = htons(OWN_PORT);
    udp->dest = htons(SERVER_PORT);
    udp->check = 0;
    udp->len = htons(i - sizeof(struct ethhdr) - 20);

    server.sll_family = AF_PACKET;
    server.ssl_ifindex = if_nametoindex("wlo1");
    server.ssl_halen = 6;

    ssize_t send_len = sendto(client_sock, buf, i, 0, server, sizeof(server));

    if (send_len < 0) {
        perror("Ошибка: не удалось отправить сообщение");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        ssize_t recv_len = recvfrom(client_sock, buf, SIZE, NULL, NULL);

        if (recv_len < 0) {
			perror("Ошибка: не удалось принять пакет");
			continue;
        }

        if ((ip->protocol == IPPROTO_UDP) && (udp->dest == htons(OWN_PORT))) {
            struct in_addr src;
		    src.s_addr = ip->saddr;

            printf("Сообщение от сервера %s:%d -> %s\n", inet_ntoa(src), ntohs(udp->source), buf + sizeof(struct ethhdr) + 20 + sizeof(struct udphdr));
        }
    }

    close(client_sock);
}