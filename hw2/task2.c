#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define SIZE 50000000
#define N 10

int arr[SIZE];
long partial_sums[N];

struct SumAndExeTime {
    long sum;
    double time;
};

struct SumAndExeTime measure_execution_time(void* (*thread_func)(void*)) {
    pthread_t threads[N];
    int thread_ids[N];
    struct timeval start, end;
    struct SumAndExeTime result;

    gettimeofday(&start, NULL);

    for (int i = 0; i < N; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    result.sum = 0;

    for (int i = 0; i < N; i++) {
        result.sum += partial_sums[i];
    }

    gettimeofday(&end, NULL);

    double execution_time = (end.tv_sec - start.tv_sec) * 1000.0;
    execution_time += (end.tv_usec - start.tv_usec) / 1000.0;

    result.time = execution_time;

    return result;
}

struct SumAndExeTime sequential_sum (int arr[]) {
        struct timeval start, end;
        struct SumAndExeTime result;

        gettimeofday(&start, NULL);

        result.sum = 0;
        for (int i = 0; i < SIZE; i++) {
                result.sum += arr[i];
        }

        gettimeofday(&end, NULL);

        double execution_time = (end.tv_sec - start.tv_sec) * 1000.0;
        execution_time += (end.tv_usec - start.tv_usec) / 1000.0;

        result.time = execution_time;

        return result;
}

void* sum_array(void* arg) {
    int thread_id = *((int*)arg);

    for (int i = thread_id * (SIZE / N); i < (thread_id + 1) * (SIZE / N); i++) {
        partial_sums[thread_id] += arr[i];
    }

    return NULL;
}


int main () {
	srand(time(NULL));

        for (int i = 0; i < SIZE; i++) {
                arr[i] = (rand() % 100) * i;
        }

        struct SumAndExeTime thread_result = measure_execution_time(sum_array);
        struct SumAndExeTime sequential_result = sequential_sum(arr);

        printf("Threads method finished in %.3f ms\nsum: %lu\n\n", thread_result.time, thread_result.sum);
        printf("Sequential method finished in %.3f ms\nsum: %lu\n", sequential_result.time, sequential_result.sum);

        return 0;
}

