#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

#define Server_to_client_queue "/myqueue1"
#define Client_to_server_queue "/myqueue2"

int main (void) {
    mqd_t mq1 = mq_open(Server_to_client_queue, O_RDWR);

    if (mq1 == (mqd_t)-1) {
        perror("Ошибка: очередь не существует");
        return 1;
    }

    mqd_t mq2 = mq_open(Client_to_server_queue, O_RDWR);

    if (mq2 == (mqd_t)-1) {
        perror("Ошибка: очередь не существует");
        return 1;
    }

    char buffer[8];
    if (mq_receive(mq1, buffer, 8, NULL) == -1) {
        perror("Ошибка: не удалось принять сообщение от сервера");
        return 1;
    }

    printf("Сообщение от сервера: %s\n", buffer);

    const char* msg = "Hello!";
    if (mq_send(mq2, msg, strlen(msg) + 1, 0) == -1) {
        perror("Ошибка: не удалось отправить сообщение серверу");
        return 1;
    }

    mq_close(mq1);
    mq_close(mq2);

    return 0;
}