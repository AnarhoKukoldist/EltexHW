#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHM_KEY 1234
#define SEM_KEY 5678
#define SHM_SIZE 1024

enum { SEM_SERVER_WRITES = 0, SEM_CLIENT_WRITES = 1, SEM_CLIENT_DONE = 2 };

void sem_signal(int semid, int semnum) {
    struct sembuf sb = {semnum, 1, 0};
    semop(semid, &sb, 1);
}

void sem_wait(int semid, int semnum) {
    struct sembuf sb = {semnum, -1, 0};
    semop(semid, &sb, 1);
}

int main() {
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    char *data = (char *)shmat(shmid, NULL, 0);

    int semid = semget(SEM_KEY, 3, 0666);
    if (semid < 0) {
        perror("semget");
        exit(1);
    }

    sem_wait(semid, SEM_SERVER_WRITES);
    printf("Client: received message: %s\n", data);

    strcpy(data, "Hello!");
    printf("Client: sent response: Hello!\n");

    sem_signal(semid, SEM_CLIENT_WRITES);

    sem_signal(semid, SEM_CLIENT_DONE);

    // Очистка
    shmdt(data);
    return 0;
}
