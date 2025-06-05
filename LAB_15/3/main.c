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
    int sig;

    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);

    if (sigprocmask(SIG_BLOCK, &set, NULL) != 0) {
        perror("Не удалось заблокировать сигнал SIGUSR1.");
        return 1;
    }

    while (1) {
        if (sigwait(&set, &sig) == 0) {
            printf("Получен сигнал SIGUSR1\n");
        }
        else {
            perror("Ошибка sigwait.");
            return 1;
        }
    }

    return 0;
}