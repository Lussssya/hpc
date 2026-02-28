#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define SIZE 20000000
#define N 10

int odd_arr[N];

struct CountAndExeTime {
    int count;
    double time;
};

int isPrime(int n) {
    if (n < 2) return 0;

    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return 0;
        }
    }

    return 1;
}

struct CountAndExeTime measure_execution_time(void* (*thread_func)(void*)) {
    pthread_t threads[N];
    int thread_ids[N];
    struct CountAndExeTime result;
    struct timeval start, end;

    gettimeofday(&start, NULL);

    for (int i = 0; i < N; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end, NULL);

    double execution_time = (end.tv_sec - start.tv_sec) * 1000.0;
    execution_time += (end.tv_usec - start.tv_usec) / 1000.0;

    result.time = execution_time;

    return result;
}

struct CountAndExeTime sequential_find_odd_count () {
        struct timeval start, end;

        gettimeofday(&start, NULL);
        struct CountAndExeTime result;

        int count = 0;
        for (int i = 1; i <= SIZE; i++) {
                if (isPrime(i)) {
                       count++;
                }
        }

        result.count = count;
        gettimeofday(&end, NULL);

        result.time = (end.tv_sec - start.tv_sec) * 1000.0;
        result.time += (end.tv_usec - start.tv_usec) / 1000.0;
        return result;
}

void* count_odd(void* arg) {
    int thread_id = *((int*)arg);

    int count = 0;
    for (int i = thread_id * (SIZE / N) + 1; i <= (thread_id + 1) * (SIZE / N); i++) {
        if (isPrime(i)) {
            count++;
        }
    }

    odd_arr[thread_id] = count;

    return NULL;
}

int main () {
        struct CountAndExeTime sequential_result = sequential_find_odd_count();
        struct CountAndExeTime thread_result = measure_execution_time(count_odd);

        int count = 0;
        for (int i = 0; i < N; i++) {
            count += odd_arr[i];
        }
        thread_result.count = count;
        
        printf("Threads method finished in %.3f ms\ncount: %d\n\n", thread_result.time, thread_result.count);
        printf("Sequential method finished in %.3f ms\ncount: %d\n", sequential_result.time, sequential_result.count);

        return 0;
}

