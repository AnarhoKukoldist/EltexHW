#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

#define Server_to_client_queue "/myqueue1"
#define Client_to_server_queue "/myqueue2"

int main (void) {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 2;
    attr.mq_msgsize = 8;
    attr.mq_curmsgs = 0;

    mqd_t mq1 = mq_open(Server_to_client_queue, O_CREAT | O_RDWR, 0666, &attr);

    if (mq1 == (mqd_t)-1) {
        perror("Ошибка: не удалось создать очередь");
        return 1;
    }

    mqd_t mq2 = mq_open(Client_to_server_queue, O_CREAT | O_RDWR, 0666, &attr);

    if (mq2 == (mqd_t)-1) {
        perror("Ошибка: не удалось создать очередь");
        return 1;
    }

    const char* msg = "Hi!";
    if (mq_send(mq1, msg, strlen(msg) + 1, 0) == -1) {
        perror("Ошибка: не удалось отправить сообщение клиенту");
        mq_close(mq1);
        mq_unlink(Server_to_client_queue);
        return 1;
    }

    char buffer[8];
    if (mq_receive(mq2, buffer, 8, NULL) == -1) {
        perror("Ошибка: не удалось принять сообщение от клиента");
        mq_close(mq2);
        mq_unlink(Client_to_server_queue);
        return 1;
    }

    printf("Сообщение от клиента: %s\n", buffer);

    mq_close(mq1);
    mq_close(mq2);
    mq_unlink(Server_to_client_queue);
    mq_unlink(Client_to_server_queue);

    return 0;
}