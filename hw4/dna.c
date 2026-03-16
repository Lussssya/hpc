#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <immintrin.h>
#include <stdint.h>
#include <inttypes.h>

#define DNA_SIZE 100000000
#define THREAD_COUNT 3

char dna[DNA_SIZE + 1];
pthread_spinlock_t spinlock_scalar;
pthread_spinlock_t spinlock_simd;

uint64_t dna_freq_scalar_threads[4];
uint64_t dna_freq_scalar[4];
uint64_t dna_freq_simd[4];
uint64_t dna_freq_simd_threads[4];

double measure_threads (void* (*thread_func)(void*)) {
        pthread_t threads[THREAD_COUNT];
        int thread_ids[THREAD_COUNT];
        struct timeval start, end;

        gettimeofday(&start, NULL);
        pthread_spin_init(&spinlock_scalar, 0);
        pthread_spin_init(&spinlock_simd, 0);

        for (int i = 0; i < THREAD_COUNT; i++) {
                thread_ids[i] = i;
                pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
        }

        for (int i = 0; i < THREAD_COUNT; i++) {
                pthread_join(threads[i], NULL);
        }

        pthread_spin_destroy(&spinlock_simd);
        pthread_spin_destroy(&spinlock_scalar);
        gettimeofday(&end, NULL);

        double execution_time = (end.tv_sec - start.tv_sec) * 1000.0;
        execution_time += (end.tv_usec - start.tv_usec) / 1000.0;

        return execution_time;
}

void* thread_scalar_compute_freq (void* arg) {
        int thread_id = *((int*)arg);
        int index;

        int chunk_size = DNA_SIZE / THREAD_COUNT;
        int remainder = DNA_SIZE % THREAD_COUNT;

        int start = thread_id * chunk_size + (thread_id < remainder ? thread_id : remainder);
        int end = start + chunk_size + (thread_id < remainder ? 1 : 0);

        for (int i = start; i < end; i++) {
                index = dna[i] == 'A' ? 0 : dna[i] == 'C' ? 1 : dna[i] == 'G' ? 2 : 3;

                pthread_spin_lock(&spinlock_scalar);
                dna_freq_scalar_threads[index] += 1;
                pthread_spin_unlock(&spinlock_scalar);
        }

        return NULL;
}

void* thread_simd_compute_freq (void* arg) {
        int thread_id = *((int*)arg);

        int chunk_size = DNA_SIZE / THREAD_COUNT;
        int remainder = DNA_SIZE % THREAD_COUNT;

        int start = thread_id * chunk_size + (thread_id < remainder ? thread_id : remainder);
        int end = start + chunk_size + (thread_id < remainder ? 1 : 0);

        int index = start;
        __m256i target_vec_a = _mm256_set1_epi8('A');
        __m256i target_vec_c = _mm256_set1_epi8('C');
        __m256i target_vec_g = _mm256_set1_epi8('G');
        __m256i target_vec_t = _mm256_set1_epi8('T');

        for (int i = start; i <= end - 32; i += 32) {
                __m256i vd = _mm256_loadu_si256((__m256i_u*)&dna[i]);
                
                __m256i cmp_a = _mm256_cmpeq_epi8(vd, target_vec_a);
                uint32_t mask_a = _mm256_movemask_epi8(cmp_a);
                int count_a = _mm_popcnt_u32(mask_a);

                __m256i cmp_c = _mm256_cmpeq_epi8(vd, target_vec_c);
                uint32_t mask_c = _mm256_movemask_epi8(cmp_c);
                int count_c = _mm_popcnt_u32(mask_c);

                __m256i cmp_g = _mm256_cmpeq_epi8(vd, target_vec_g);
                uint32_t mask_g = _mm256_movemask_epi8(cmp_g);
                int count_g = _mm_popcnt_u32(mask_g);

                __m256i cmp_t = _mm256_cmpeq_epi8(vd, target_vec_t);
                uint32_t mask_t = _mm256_movemask_epi8(cmp_t);
                int count_t = _mm_popcnt_u32(mask_t);


                pthread_spin_lock(&spinlock_simd);
                dna_freq_simd_threads[0] += count_a;
                dna_freq_simd_threads[1] += count_c;
                dna_freq_simd_threads[2] += count_g;
                dna_freq_simd_threads[3] += count_t;
                pthread_spin_unlock(&spinlock_simd);

                index = i + 32;
        }

        while (index < end) {
                int identified_cell;
                identified_cell = dna[index] == 'A' ? 0 : dna[index] == 'C' ? 1 : dna[index] == 'G' ? 2 : 3;

                pthread_spin_lock(&spinlock_simd);
                dna_freq_simd_threads[identified_cell] += 1;
                pthread_spin_unlock(&spinlock_simd);
                
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

void scalar_compute_freq () {
        int index;
        for (int i = 0; i < DNA_SIZE; i++) {
                index = dna[i] == 'A' ? 0 : dna[i] == 'C' ? 1 : dna[i] == 'G' ? 2 : 3;

                dna_freq_scalar[index] += 1;
        }
}

void simd_compute_freq () {
        int index = 0;

        __m256i target_vec_a = _mm256_set1_epi8('A');
        __m256i target_vec_c = _mm256_set1_epi8('C');
        __m256i target_vec_g = _mm256_set1_epi8('G');
        __m256i target_vec_t = _mm256_set1_epi8('T');

        for (int i = 0; i <= DNA_SIZE - 32; i += 32) {
                __m256i vd = _mm256_loadu_si256((__m256i_u*)&dna[i]);

                __m256i cmp_a = _mm256_cmpeq_epi8(vd, target_vec_a);
                uint32_t mask_a = _mm256_movemask_epi8(cmp_a);
                long count_a = _mm_popcnt_u32(mask_a);

                __m256i cmp_c = _mm256_cmpeq_epi8(vd, target_vec_c);
                uint32_t mask_c = _mm256_movemask_epi8(cmp_c);
                long count_c = _mm_popcnt_u32(mask_c);

                __m256i cmp_g = _mm256_cmpeq_epi8(vd, target_vec_g);
                uint32_t mask_g = _mm256_movemask_epi8(cmp_g);
                long count_g = _mm_popcnt_u32(mask_g);

                __m256i cmp_t = _mm256_cmpeq_epi8(vd, target_vec_t);
                uint32_t mask_t = _mm256_movemask_epi8(cmp_t);
                long count_t = _mm_popcnt_u32(mask_t);


                dna_freq_simd[0] += count_a;
                dna_freq_simd[1] += count_c;
                dna_freq_simd[2] += count_g;
                dna_freq_simd[3] += count_t;

                index = i + 32;
        }

        while (index < DNA_SIZE) {
                int identified_cell;
                identified_cell = dna[index] == 'A' ? 0 : dna[index] == 'C' ? 1 : dna[index] == 'G' ? 2 : 3;

                dna_freq_simd[identified_cell] += 1;
                index++;
        }
}

void generateDna () {
        char arr[4] = {'A', 'C', 'G', 'T'};
        int rand_num;
        for (int i = 0; i < DNA_SIZE; i++) {
                rand_num = rand() % 4;
                dna[i] = arr[rand_num];
        }

        dna[DNA_SIZE] = '\0';
}

int main () {
        srand(time(NULL));
        generateDna();

        memset(dna_freq_scalar_threads, 0, sizeof(dna_freq_scalar_threads));
        memset(dna_freq_scalar, 0, sizeof(dna_freq_scalar));
        memset(dna_freq_simd, 0, sizeof(dna_freq_simd));
        memset(dna_freq_simd_threads, 0, sizeof(dna_freq_simd_threads));

        double threads_scalar = measure_threads(thread_scalar_compute_freq);
        double scalar = measure_func_time(scalar_compute_freq);
        double simd = measure_func_time(simd_compute_freq);
        double threads_simd = measure_threads(thread_simd_compute_freq);

        printf("Execution time Threads + Scalar: %.2f ms\n", threads_scalar);
        printf("Results: A:%" PRIu64 " C:%" PRIu64 " G:%" PRIu64 " T:%" PRIu64 "\n\n",
                dna_freq_scalar_threads[0],
                dna_freq_scalar_threads[1],
                dna_freq_scalar_threads[2],
                dna_freq_scalar_threads[3]);

        printf("Execution time Scalar: %.2f ms\n", scalar);
        printf("Results: A:%" PRIu64 " C:%" PRIu64 " G:%" PRIu64 " T:%" PRIu64 "\n\n",
                dna_freq_scalar[0],
                dna_freq_scalar[1],
                dna_freq_scalar[2],
                dna_freq_scalar[3]);

        printf("Execution time SIMD: %.2f ms\n", simd);
        printf("Results: A:%" PRIu64 " C:%" PRIu64 " G:%" PRIu64 " T:%" PRIu64 "\n\n",
                dna_freq_simd[0],
                dna_freq_simd[1],
                dna_freq_simd[2],
                dna_freq_simd[3]);

        printf("Execution time Threads + SIMD: %.2f ms\n", threads_simd);
        printf("Results: A:%" PRIu64 " C:%" PRIu64 " G:%" PRIu64 " T:%" PRIu64 "\n\n",
                dna_freq_simd_threads[0],
                dna_freq_simd_threads[1],
                dna_freq_simd_threads[2],
                dna_freq_simd_threads[3]);

        return 0;
}
