#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main (void) {
    int fd[2];
    pipe(fd);

    int pid = fork();

    if (pid == -1) {
        perror("Ошибка: не удлось породить процесс");
        return 1;
    }
    else if (pid == 0) {
        close(fd[1]);
        char buf[3];
        ssize_t size = read(fd[0], buf, 3);

        if (size > 0) {
            buf[size] = '\0';
            printf("Получено от родительского процесса: %s\n", buf);

        }
        close(fd[0]);
        exit(0);
    }
    else {
        close(fd[0]);
        const char* str = "Hi!";
        write(fd[1], str, strlen(str));
        close(fd[1]);
        wait(NULL);
    }

    return 0;
}