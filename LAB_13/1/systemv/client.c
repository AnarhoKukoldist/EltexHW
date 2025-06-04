#include <stdio.h>
#include <string.h>
#include <sys/msg.h>

#define QUEUE_KEY 2005

struct message {
    long mtype;
    char mtext[8];
};

int main (void) {
    int msgid = msgget(QUEUE_KEY, 0666);

    if (msgid == -1) {
        perror("Ошибка: очередь не существует");
        return 1;
    }

    struct message msg;

    if (msgrcv(msgid, &msg, 8, 1, 0) == -1) {
        perror("Ошибка: не удалось принять сообщение от сервера");
        return 1;
    }

    printf("Сообщение от сервера: %s\n", msg.mtext);

    msg.mtype = 2;
    strcpy(msg.mtext, "Hello!");

    if (msgsnd(msgid, &msg, strlen(msg.mtext) + 1, 0) == -1) {
        perror("Ошибка: не удалось отправить сообщение серверу");
        return 1;
    }

    return 0;
}