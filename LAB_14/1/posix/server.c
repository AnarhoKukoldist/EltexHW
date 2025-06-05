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
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, 1024);
    char *data = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *sem_write = sem_open(SEM_WRITE, O_CREAT, 0666, 0);
    sem_t *sem_read = sem_open(SEM_READ, O_CREAT, 0666, 0);

    strcpy(data, "Hi!");
    sem_post(sem_write);

    sem_wait(sem_read);
    printf("Server got: %s\n", data);

    munmap(data, 1024);
    close(shm_fd);
    shm_unlink(SHM_NAME);
    sem_close(sem_write);
    sem_close(sem_read);
    sem_unlink(SEM_WRITE);
    sem_unlink(SEM_READ);
    return 0;
}
