#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *routine (void* args) {
    int index = *(int *)args;
    printf("Поток №%d\n", index);
    return NULL;
}

int main (void) {
    pthread_t threads[5];
    int index[5] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++) {
        if (pthread_create(threads + i, NULL, routine, index + i)) {
            perror("Ошибка: не удалось создать поток");
            return 1;
        }
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

