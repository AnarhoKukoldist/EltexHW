#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define QUEUE_KEY 2005

struct message {
    long mtype;
    char mtext[8];
};

int main (void) {
    int msgid = msgget(QUEUE_KEY, IPC_CREAT | 0666);

    if (msgid == -1) {
        perror("Ошибка: не удалось создать очередь сооющений");
        return 1;
    }

    struct message msg;
    msg.mtype = 1;
    strcpy(msg.mtext, "Hi!");

    if (msgsnd(msgid, &msg, strlen(msg.mtext) + 1, 0) == -1) {
        perror("Ошибка: не удалось отправить сообщение клиенту");
        msgctl(msgid, IPC_RMID, NULL);
        return 1;
    }

    if (msgrcv(msgid, &msg, 8, 2, 0) == -1) {
        perror("Ошибка: не удалось принять сообщение от клиента");
        msgctl(msgid, IPC_RMID, NULL);
        return 1;
    }

    printf("Сообщение от клиента: %s\n", msg.mtext);

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}