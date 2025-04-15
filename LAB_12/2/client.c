#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main (void) {
    int fd = open("/tmp/myfifo", O_RDONLY);

    if (fd == -1) {
        perror("Ошибка: не удалось получить доступ к именованному каналу");
        return -1;
    }

    char buf[3];
    ssize_t size = read(fd, buf, 3);

    if (size > 0) {
        buf[size] = '\0';
        printf("Получено от сервера: %s\n", buf);
    }

    close(fd);
    return 0;
}