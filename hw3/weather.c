#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define S 4
pthread_barrier_t barrier;
int temperatures[S];

void cycle (void* (*thread_func)(void*)) {
    pthread_t sensors[S];
    int sensor_ids[S];

    for (int i = 0; i < S; i++) {
        sensor_ids[i] = i;
        pthread_create(&sensors[i], NULL, thread_func, &sensor_ids[i]);
    }

    for (int i = 0; i < S; i++) {
        pthread_join(sensors[i], NULL);
    }
}

double averageTemp () {
    double sum = 0;
    for (int i = 0; i < S; i++) {
        sum += temperatures[i];
    }

    return sum / S;
}

void* thread_func (void* args) {
    int sensor_id = *(int*) args;

    int temperature = (rand() % (36 + 7 + 1)) - 7;
    temperatures[sensor_id] = temperature;

    printf("Sensor %d detected %dC and is waiting\n", sensor_id, temperature);
    int status = pthread_barrier_wait(&barrier);
    printf("Sensor %d passed the barrier\n", sensor_id);

    if (status == PTHREAD_BARRIER_SERIAL_THREAD) {
	printf("The average temperatures is %.2fC!\n", averageTemp());
    }
    return NULL;
}

int main () {
    srand(time(NULL));
    pthread_barrier_init(&barrier, NULL, S);
    cycle(thread_func);
    pthread_barrier_destroy(&barrier);

    return 0;
}
