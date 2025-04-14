#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_NUM 5
#define MAX 180000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_spinlock_t spinlock;
int local_num[THREAD_NUM];
int counter = 0;

void serial (void) {
    for (int i = 0; i < MAX; i++) {
        counter++;
    }
}

void* with_mutex (void* args) {
    int local_max = MAX / THREAD_NUM;
    for (int i = 0; i < local_max; i++) {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void* with_spinlock (void* args) {
    int local_max = MAX / THREAD_NUM;
    for (int i = 0; i < local_max; i++) {
        pthread_spin_lock(&spinlock);
        counter++;
        pthread_spin_unlock(&spinlock);
    }

    return NULL;
}

void* with_mapreduce (void* args) {
    int index = *(int *)args;
    int local_max = MAX / THREAD_NUM, i = 0;
    while (i < local_max) {
        i++;
    }
    local_num[index] = i;

    return NULL;
}

int main (void) {
    serial();
    printf("Последовательная версия: counter = %d\n", counter);

    pthread_t threads[THREAD_NUM];

    counter = 0;
    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(threads + i, NULL, with_mutex, NULL)) {
            perror("Ошибка: не удалось породить поток");
            return 1;
        }
    }
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Версия с mutex: counter = %d\n", counter);

    counter = 0;
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(threads + i, NULL, with_spinlock, NULL)) {
            perror("Ошибка: не удалось породить поток");
            return 1;
        }
    }
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Версия с spinlock: counter = %d\n", counter);
    pthread_spin_destroy(&spinlock);

    counter = 0;
    int index[THREAD_NUM] = {0, 1, 2, 3, 4};
    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(threads + i, NULL, with_mapreduce, index + i)) {
            perror("Ошибка: не удалось породить поток");
            return 1;
        }
    }
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(threads[i], NULL);
        counter += local_num[i];
    }
    printf("Версия с mapreduce: counter = %d\n", counter);

    return 0;
}