#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main (void) {
    FILE* file = fopen("../1/pid.txt", "w"); // чтобы программа из первого задания получила pid

    if (!file) {
        perror("Не удалось открыть файл для записи pid.");
        return 1;
    }

    fprintf(file, "%d", getpid());
    fclose(file);

    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    if (sigprocmask(SIG_BLOCK, &set, NULL) != 0) {
        perror("Не удалось заблокировать сигнал SIGINT.");
        return 1;
    }

    while (1);

    return 0;
}