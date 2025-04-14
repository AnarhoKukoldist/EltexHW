#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define ROOM_COUNT 5
#define BUYER_COUNT 3

int rooms[ROOM_COUNT];
pthread_mutex_t room_mutexes[ROOM_COUNT];

int buyers_done = 0;
pthread_mutex_t done_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t loader_thread;

typedef struct {
    int id;
    int need;
} Buyer;

void* buyer_func(void* arg) {
    Buyer* buyer = (Buyer*)arg;
    printf("Покупатель %d: начал с потребностью %d\n", buyer->id, buyer->need);

    while (1) {
        int satisfied = 0;

        for (int i = 0; i < ROOM_COUNT; ++i) {
            if (pthread_mutex_trylock(&room_mutexes[i]) == 0) {
                int taken = (rooms[i] < buyer->need) ? rooms[i] : buyer->need;

                printf("Покупатель %d: зашел в комнату %d, товаров там было %d, взял %d\n",
                       buyer->id, i, rooms[i], taken);

                rooms[i] -= taken;
                buyer->need -= taken;

                printf("Покупатель %d: осталось потребности %d\n", buyer->id, buyer->need);

                pthread_mutex_unlock(&room_mutexes[i]);

                if (buyer->need == 0) {
                    printf("Покупатель %d: насытился и завершает работу\n", buyer->id);
                    pthread_mutex_lock(&done_mutex);
                    buyers_done++;
                    pthread_mutex_unlock(&done_mutex);
                    free(buyer);
                    return NULL;
                }

                satisfied = 1;
                break;
            }
        }

        if (!satisfied) {
            printf("Покупатель %d: не смог зайти ни в одну комнату, спит 2 секунды\n", buyer->id);
        } else {
            printf("Покупатель %d: спит 2 секунды перед следующей попыткой\n", buyer->id);
        }

        sleep(2);
    }
}

void* loader_func(void* arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&done_mutex);
        if (buyers_done >= BUYER_COUNT) {
            pthread_mutex_unlock(&done_mutex);
            break;
        }
        pthread_mutex_unlock(&done_mutex);

        for (int i = 0; i < ROOM_COUNT; ++i) {
            if (pthread_mutex_trylock(&room_mutexes[i]) == 0) {
                rooms[i] += 500;
                printf("Погрузчик: зашел в комнату %d, добавил 500 товаров. Теперь там %d\n",
                       i, rooms[i]);
                pthread_mutex_unlock(&room_mutexes[i]);
                break;
            }
        }

        sleep(1);
    }

    printf("Погрузчик: все покупатели насытились, завершает работу\n");
    return NULL;
}

int main() {
    srand(time(NULL));

    for (int i = 0; i < ROOM_COUNT; ++i) {
        rooms[i] = 500;
        pthread_mutex_init(&room_mutexes[i], NULL);
    }

    pthread_t buyers[BUYER_COUNT];

    for (int i = 0; i < BUYER_COUNT; ++i) {
        Buyer* buyer = malloc(sizeof(Buyer));
        buyer->id = i + 1;
        buyer->need = 10000 + rand() % 2001;

        pthread_create(&buyers[i], NULL, buyer_func, buyer);
    }

    pthread_create(&loader_thread, NULL, loader_func, NULL);

    for (int i = 0; i < BUYER_COUNT; ++i) {
        pthread_join(buyers[i], NULL);
    }

    pthread_join(loader_thread, NULL);

    for (int i = 0; i < ROOM_COUNT; ++i) {
        pthread_mutex_destroy(&room_mutexes[i]);
    }

    pthread_mutex_destroy(&done_mutex);

    printf("Главный поток: все завершено\n");
    return 0;
}
