#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define N 4
pthread_barrier_t barrier;

void cycle (void* (*thread_func)(void*)) {
    pthread_t players[N];
    int player_ids[N];

    pthread_barrier_init(&barrier, NULL, N);

    for (int i = 0; i < N; i++) {
        player_ids[i] = i;
        pthread_create(&players[i], NULL, thread_func, &player_ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(players[i], NULL);
    }
    
    pthread_barrier_destroy(&barrier);
}

void* thread_func (void* args) {
    int player_id = *((int*)args);
    int time = rand() % 3 + 1;
    sleep(time);
    printf("Player %d is waiting\n", player_id);
    int status = pthread_barrier_wait(&barrier);
    printf("Player %d passed the barrier\n", player_id);

    if (status == PTHREAD_BARRIER_SERIAL_THREAD) {
	printf("Game started!!\n");
    }
    return NULL;
}

int main () {
    srand(time(NULL));
    cycle(thread_func);
    return 0;
}
