#include <stdio.h>
#include <signal.h>

int main (void) {
    FILE* file = fopen("pid.txt", "r");

    if (!file) {
        perror("Не удалось открыть файл для чтения pid");
        return 1;
    }

    pid_t pid;
    fscanf(file, "%d", &pid);
    fclose(file);

    kill(pid, SIGUSR1);

    return 0;
}