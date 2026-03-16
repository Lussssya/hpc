#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/time.h>
#include <immintrin.h>

#define BUFFER_SIZE (200 * 1024 * 1024)
#define THREAD_COUNT 4

char* buffer_thread;
char* buffer_simd;
char* buffer_thread_simd;

double measure_threads (void* (*thread_func)(void*)) {
        pthread_t threads[THREAD_COUNT];
        int thread_ids[THREAD_COUNT];
        struct timeval start, end;

        gettimeofday(&start, NULL);

        for (int i = 0; i < THREAD_COUNT; i++) {
                thread_ids[i] = i;
                pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
        }

        for (int i = 0; i < THREAD_COUNT; i++) {
                pthread_join(threads[i], NULL);
        }

        gettimeofday(&end, NULL);

        double execution_time = (end.tv_sec - start.tv_sec) * 1000.0;
        execution_time += (end.tv_usec - start.tv_usec) / 1000.0;

        return execution_time;
}

void* modify_buffer_thread (void* arg) {
        int thread_id = *((int*)arg);
        int index;

        int chunk_size = BUFFER_SIZE / THREAD_COUNT;
        int remainder = BUFFER_SIZE % THREAD_COUNT;

        int start = thread_id * chunk_size + (thread_id < remainder ? thread_id : remainder);
        int end = start + chunk_size + (thread_id < remainder ? 1 : 0);

        for (int i = start; i < end; i++) {
                if (islower(buffer_thread[i])) {
                        buffer_thread[i] = toupper(buffer_thread[i]);
                }
        }

        return NULL;
}

void modify_buffer_simd () {
        int index = 0;

        __m256i lower_bound = _mm256_set1_epi8('a' - 1);
        __m256i upper_bound = _mm256_set1_epi8('z' + 1);
        __m256i diff = _mm256_set1_epi8(32);

        for (int i = 0; i <= BUFFER_SIZE - 32; i += 32) {
                __m256i chars = _mm256_loadu_si256((__m256i_u*)&buffer_simd[i]);
                __m256i ge_a = _mm256_cmpgt_epi8(chars, lower_bound);
                __m256i le_z = _mm256_cmpgt_epi8(upper_bound, chars);
                __m256i lower_mask = _mm256_and_si256(ge_a, le_z);
                __m256i uppercase = _mm256_sub_epi8(chars, diff);
                __m256i result = _mm256_blendv_epi8(chars, uppercase, lower_mask);
                _mm256_storeu_si256((__m256i*)&buffer_simd[i], result);

                index = i + 32;
        }

        while (index < BUFFER_SIZE) {
                if (islower(buffer_simd[index])) {
                        buffer_simd[index] = toupper(buffer_simd[index]);
                }
                index++;
        }
}

void* modify_buffer_thread_simd (void* arg) {
        int thread_id = *((int*)arg);

        int chunk_size = BUFFER_SIZE / THREAD_COUNT;
        int remainder = BUFFER_SIZE % THREAD_COUNT;

        int start = thread_id * chunk_size + (thread_id < remainder ? thread_id : remainder);
        int end = start + chunk_size + (thread_id < remainder ? 1 : 0);

        int index = start;
        __m256i lower_bound = _mm256_set1_epi8('a' - 1);
        __m256i upper_bound = _mm256_set1_epi8('z' + 1);
        __m256i diff = _mm256_set1_epi8(32);

        for (int i = start; i <= end - 32; i += 32) {
                __m256i chars = _mm256_loadu_si256((__m256i_u*)&buffer_thread_simd[i]);
                __m256i ge_a = _mm256_cmpgt_epi8(chars, lower_bound);
                __m256i le_z = _mm256_cmpgt_epi8(upper_bound, chars);
                __m256i lower_mask = _mm256_and_si256(ge_a, le_z);
                __m256i uppercase = _mm256_sub_epi8(chars, diff);
                __m256i result = _mm256_blendv_epi8(chars, uppercase, lower_mask);
                _mm256_storeu_si256((__m256i*)&buffer_thread_simd[i], result);

                index = i + 32;
        }

        while (index < end) {
                if (islower(buffer_thread_simd[index])) {
                        buffer_thread_simd[index] = toupper(buffer_thread_simd[index]);
                }
                index++;
        }

        return NULL;
}

double measure_func_time (void (*func)(void)) {
        struct timeval start, end;
        gettimeofday(&start, NULL);
        func();
        gettimeofday(&end, NULL);

        double execution_time = (end.tv_sec - start.tv_sec) * 1000.0;
        execution_time += (end.tv_usec - start.tv_usec) / 1000.0;

        return execution_time;
}

char* generate_random_buffer () {
        char* buffer = (char*)malloc(BUFFER_SIZE + 1);

        char valid_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ";
        int valid_count = strlen(valid_chars);
    
        for (size_t i = 0; i < BUFFER_SIZE; i++) {
                buffer[i] = valid_chars[rand() % valid_count];
        }

        buffer[BUFFER_SIZE] = '\0';
        return buffer;
}

int main () {
        srand(time(NULL));
        buffer_thread = generate_random_buffer();
        buffer_simd = malloc(BUFFER_SIZE + 1);
        buffer_thread_simd = malloc(BUFFER_SIZE + 1);

        if (!buffer_thread || !buffer_simd || !buffer_thread_simd) {
        printf("Memory allocation failed!\n");
        return 1;
        }

        memcpy(buffer_simd, buffer_thread, BUFFER_SIZE + 1);
        memcpy(buffer_thread_simd, buffer_thread, BUFFER_SIZE + 1);

        double threads = measure_threads(modify_buffer_thread);
        double simd = measure_func_time(modify_buffer_simd);
        double threads_simd = measure_threads(modify_buffer_thread_simd);

        printf("threads exe time: %.2f\n\n", threads);
        printf("simd exe time: %.2f\n\n", simd);
        printf("threads+simd time: %.2f\n\n", threads_simd);

        free(buffer_thread);
        free(buffer_simd);
        free(buffer_thread_simd);

        return 0;
}
