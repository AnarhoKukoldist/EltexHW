#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int main (void) {
    if (mkfifo("/tmp/myfifo", 0666) == -1) {
        if (errno != EEXIST) {
            perror("Ошибка: не удалось создать именованный канал");
            return 1;
        }
        
    }

    int fd = open("/tmp/myfifo", O_WRONLY);

    if (fd == -1) {
        perror("Ошибка: не удалось получить доступ к именованному каналу");
        return 1;
    }

    write(fd, "Hi!", 3);
    close(fd);
    return 0;
}