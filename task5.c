#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#define SIZE 20000000
#define N 4
#define RED "\033[31m"
#define RESET "\033[0m"

void run_threads (void* (*thread_func)(void*)) {
    pthread_t threads[N];
    int thread_ids[N];
    
    for (int i = 0; i < N; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
}

void* heavy_iteration (void* arg) {
    int thread_id = *((int*)arg);

    for (int i = 0; i < SIZE; i++) {
	sleep(1);
        printf("running thread %s%d%s on CPU: %s%d%s\n", RED, thread_id,RESET, RED, sched_getcpu(), RESET);
    }

    return NULL;
}

int main () {
        run_threads(heavy_iteration);

        return 0;
}

