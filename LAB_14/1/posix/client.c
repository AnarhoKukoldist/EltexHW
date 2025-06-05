#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

#define SHM_NAME "/myshm"
#define SEM_WRITE "/sem_write"
#define SEM_READ "/sem_read"

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    char *data = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *sem_write = sem_open(SEM_WRITE, 0);
    sem_t *sem_read = sem_open(SEM_READ, 0);

    sem_wait(sem_write);
    printf("Client got: %s\n", data);
    strcpy(data, "Hello!");
    sem_post(sem_read); // сигнал серверу

    munmap(data, 1024);
    close(shm_fd);
    sem_close(sem_write);
    sem_close(sem_read);
    return 0;
}
