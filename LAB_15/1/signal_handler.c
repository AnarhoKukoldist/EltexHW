#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void handler () {
    printf("Получен сигнал SIGUSR1\n");
}

int main (void) {
    FILE* file = fopen("pid.txt", "w"); // чтобы вторая программа сразу знала pid

    if (!file) {
        perror("Не удалось открыть файл для записи pid");
        return 1;
    }

    fprintf(file, "%d", getpid());
    fclose(file);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) != 0) {
        perror("Не удалось установить новый обработчик сигнала.");
        return 1;
    }

    while (1);

    return 0;
}