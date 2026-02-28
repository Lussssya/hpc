#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <limits.h>
#include <sys/time.h>


#define SIZE 50000000
#define N 4

int arr[SIZE];
int max_arr[N];

struct MaxAndExeTime {
        int max;
        double time;
};

struct MaxAndExeTime measure_execution_time(void* (*thread_func)(void*)) {
    pthread_t threads[N];
    int thread_ids[N];
    struct timeval start, end;
    struct MaxAndExeTime result;

    gettimeofday(&start, NULL);

    for (int i = 0; i < N; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    int max = INT_MIN;
    for (int i = 0; i < N; i++) {
        if (max < max_arr[i]) {
            max = max_arr[i];
        }
    }

    result.max = max;

    gettimeofday(&end, NULL);

    double execution_time = (end.tv_sec - start.tv_sec) * 1000.0;
    execution_time += (end.tv_usec - start.tv_usec) / 1000.0;

    result.time = execution_time;

    return result;
}

struct MaxAndExeTime sequential_find_max (int arr[]) {
        struct MaxAndExeTime result;
        struct timeval start, end;

        gettimeofday(&start, NULL);

        int max = INT_MIN;
        for (int i = 0; i < SIZE; i++) {
                if (max < arr[i]) {
                       max = arr[i];
                }
        }
        result.max = max;

        gettimeofday(&end, NULL);

        result.time = (end.tv_sec - start.tv_sec) * 1000.0;
        result.time += (end.tv_usec - start.tv_usec) / 1000.0;
        return result;
}

void* max_array(void* arg) {
    int thread_id = *((int*)arg);

    int max = INT_MIN;
    for (int i = thread_id * (SIZE / N); i < (thread_id + 1) * (SIZE / N); i++) {
        if (max < arr[i]) {
            max = arr[i];
        }
    }

    max_arr[thread_id] = max;

    return NULL;
}

int main () {
        srand(time(NULL));

        for (int i = 0; i < SIZE; i++) {
                arr[i] = (rand() % 100) * i;
        }

        struct MaxAndExeTime thread_result = measure_execution_time(max_array);
        struct MaxAndExeTime sequential_result = sequential_find_max(arr);

        printf("Threads method finished in %.3f ms\nmax: %d\n\n", thread_result.time, thread_result.max);
        printf("Sequential method finished in %.3f ms\nmax: %d\n", sequential_result.time, sequential_result.max);

        return 0;
}
