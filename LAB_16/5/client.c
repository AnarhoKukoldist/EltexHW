#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

int main (void) {
	int client_sock;
	char buf[65536];

	client_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

	if (client_sock < 0) {
		perror("Ошибка: не удалось создать сокет");
		exit(EXIT_FAILURE);
	}

	while (1) {
		ssize_t recv_len = recvfrom(client_sock, buf, sizeof(buf), 0, NULL, NULL);

		if (recv_len < 0) {
			perror("Ошибка: не удалось принять пакет");
			continue;
		}

		struct iphdr* ip = (struct iphdr *)buf;
		struct udphdr* udp = (struct udphdr *)(buf + ip->ihl * 4);
		struct in_addr src, dst;

		src.s_addr = ip->saddr;
		dst.s_addr = ip->daddr;

		printf("%s:%d -> %s:%d: %s\n", inet_ntoa(src), ntohs(udp->source), inet_ntoa(dst), ntohs(udp->dest), buf + ip->ihl * 4 + sizeof(struct udphdr));
	}
}
