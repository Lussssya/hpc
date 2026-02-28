#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define M 4
pthread_barrier_t barrier;

void cycle (void* (*thread_func)(void*)) {
    pthread_t threads[M];
    int thread_ids[M];

    for (int i = 0; i < M; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }

    for (int i = 0; i < M; i++) {
        pthread_join(threads[i], NULL);
    }
}

void* thread_func (void* args) {
    int thread_id = *(int*) args;

    for (int i = 0; i < 3; i++) {
    	sleep(rand() % 3 + 1);

    	printf("Thread %d waiting at barrier %d\n", thread_id, i + 1);
    	pthread_barrier_wait(&barrier);
    	printf("Thread %d passed the barrier %d\n", thread_id, i + 1);
    }

    return NULL;
}


int main () {
    srand(time(NULL));
    pthread_barrier_init(&barrier, NULL, M);

    cycle(thread_func);

    pthread_barrier_destroy(&barrier);

    return 0;
}
