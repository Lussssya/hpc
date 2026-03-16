#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <immintrin.h>

#define THREAD_COUNT 3

int WIDTH = 0;
int HEIGHT = 0;
int SIZE = 0;

struct Rgb {
        unsigned char r;
        unsigned char g;
        unsigned char b;
};

struct Rgb *p6_thread = NULL;
struct Rgb *p6_simd = NULL;
struct Rgb *p6_simd_thread = NULL;

double measure_threads(void* (*thread_func)(void*)) {
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

void* grayscale_thread_func(void* arg) {
        int thread_id = *((int*)arg);

        int chunk_size = SIZE / THREAD_COUNT;
        int remainder = SIZE % THREAD_COUNT;

        int start = thread_id * chunk_size + (thread_id < remainder ? thread_id : remainder);
        int end = start + chunk_size + (thread_id < remainder ? 1 : 0);

        for (int i = start; i < end; i++) {
                unsigned char gray = (unsigned char)(
                0.299 * p6_thread[i].r +
                0.587 * p6_thread[i].g +
                0.114 * p6_thread[i].b
                );

                p6_thread[i].r = gray;
                p6_thread[i].g = gray;
                p6_thread[i].b = gray;
        }

        return NULL;
}

void grayscale_simd_func() {
        int index = 0;

        __m256 wr = _mm256_set1_ps(0.299f);
        __m256 wg = _mm256_set1_ps(0.587f);
        __m256 wb = _mm256_set1_ps(0.114f);

        for (int i = 0; i <= SIZE - 8; i += 8) {
                __m256 r = _mm256_set_ps(p6_simd[i+7].r, p6_simd[i+6].r, p6_simd[i+5].r, p6_simd[i+4].r, p6_simd[i+3].r, p6_simd[i+2].r, p6_simd[i+1].r, p6_simd[i].r);
                __m256 g = _mm256_set_ps(p6_simd[i+7].g, p6_simd[i+6].g, p6_simd[i+5].g, p6_simd[i+4].g, p6_simd[i+3].g, p6_simd[i+2].g, p6_simd[i+1].g, p6_simd[i].g);
                __m256 b = _mm256_set_ps(p6_simd[i+7].b, p6_simd[i+6].b, p6_simd[i+5].b, p6_simd[i+4].b, p6_simd[i+3].b, p6_simd[i+2].b, p6_simd[i+1].b, p6_simd[i].b);

                __m256 gray = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(r, wr), _mm256_mul_ps(g, wg)),_mm256_mul_ps(b, wb));

                float result[8];
                _mm256_storeu_ps(result, gray);

                for (int j = 0; j < 8; j++) {
                        unsigned char gray_value = (unsigned char)result[j];
                        p6_simd[i+j].r = gray_value;
                        p6_simd[i+j].g = gray_value;
                        p6_simd[i+j].b = gray_value;
                }

                index = i + 8;
        }
        
        while (index < SIZE) {
                unsigned char gray = (unsigned char)(0.299 * p6_simd[index].r + 0.587 * p6_simd[index].g + 0.114 * p6_simd[index].b);

                p6_simd[index].r = gray;
                p6_simd[index].g = gray;
                p6_simd[index].b = gray;

                index++;
        }
}

void* grayscale_simd_thread_func(void* arg) {
        int thread_id = *((int*)arg);

        int chunk_size = SIZE / THREAD_COUNT;
        int remainder = SIZE % THREAD_COUNT;

        int start = thread_id * chunk_size + (thread_id < remainder ? thread_id : remainder);
        int end = start + chunk_size + (thread_id < remainder ? 1 : 0);

        int index = start;

        __m256 wr = _mm256_set1_ps(0.299f);
        __m256 wg = _mm256_set1_ps(0.587f);
        __m256 wb = _mm256_set1_ps(0.114f);

        for (int i = start; i <= end - 8; i += 8) {
                __m256 r = _mm256_set_ps(p6_simd_thread[i+7].r, p6_simd_thread[i+6].r, p6_simd_thread[i+5].r, p6_simd_thread[i+4].r, p6_simd_thread[i+3].r, p6_simd_thread[i+2].r, p6_simd_thread[i+1].r, p6_simd_thread[i].r);
                __m256 g = _mm256_set_ps(p6_simd_thread[i+7].g, p6_simd_thread[i+6].g, p6_simd_thread[i+5].g, p6_simd_thread[i+4].g, p6_simd_thread[i+3].g, p6_simd_thread[i+2].g, p6_simd_thread[i+1].g, p6_simd_thread[i].g);
                __m256 b = _mm256_set_ps(p6_simd_thread[i+7].b, p6_simd_thread[i+6].b, p6_simd_thread[i+5].b, p6_simd_thread[i+4].b, p6_simd_thread[i+3].b, p6_simd_thread[i+2].b, p6_simd_thread[i+1].b, p6_simd_thread[i].b);

                __m256 gray = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(r, wr), _mm256_mul_ps(g, wg)),_mm256_mul_ps(b, wb));

                float result[8];
                _mm256_storeu_ps(result, gray);

                for (int j = 0; j < 8; j++) {
                        unsigned char gray_value = (unsigned char)result[j];
                        p6_simd_thread[i+j].r = gray_value;
                        p6_simd_thread[i+j].g = gray_value;
                        p6_simd_thread[i+j].b = gray_value;
                }

                index = i + 8;
        }
        
        while (index < end) {
                unsigned char gray = (unsigned char)(0.299 * p6_simd_thread[index].r + 0.587 * p6_simd_thread[index].g + 0.114 * p6_simd_thread[index].b);

                p6_simd_thread[index].r = gray;
                p6_simd_thread[index].g = gray;
                p6_simd_thread[index].b = gray;

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

int readP6(const char *filename) {
        FILE *fp = fopen(filename, "rb");
        if (!fp) {
                perror("Error opening file");
                return -1;
        }

        char magic[3];
        int maxval;

        if (fscanf(fp, "%2s", magic) != 1) {
                printf("Error reading magic number\n");
                fclose(fp);
                return -1;
        }

        if (strcmp(magic, "P6") != 0) {
                printf("Error: Not a P6 PPM file\n");
                fclose(fp);
                return -1;
        }

        int ch = fgetc(fp);
        while (ch == '#') {
                while (fgetc(fp) != '\n');
                ch = fgetc(fp);
        }
        ungetc(ch, fp);

        if (fscanf(fp, "%d %d %d", &WIDTH, &HEIGHT, &maxval) != 3) {
                printf("Error reading image size\n");
                fclose(fp);
                return -1;
        }

        if (maxval != 255) {
                printf("Unsupported maxval: %d (must be 255)\n", maxval);
                fclose(fp);
                return -1;
        }

        fgetc(fp);

        SIZE = WIDTH * HEIGHT;

        printf("Reading PPM: %dx%d, maxval=%d\n", WIDTH, HEIGHT, maxval);

        p6_thread = malloc(SIZE * sizeof(struct Rgb));
        p6_simd = malloc(SIZE * sizeof(struct Rgb));
        p6_simd_thread = malloc(SIZE * sizeof(struct Rgb));

        if (!p6_thread || !p6_simd || !p6_simd_thread) {
                printf("Error: Memory allocation failed\n");
                free(p6_thread);
                free(p6_simd);
                free(p6_simd_thread);
                fclose(fp);
                return -1;
        }

        size_t bytes_read = fread(p6_thread, sizeof(struct Rgb), SIZE, fp);

        if (bytes_read != (size_t)SIZE) {
                printf("Error: Only read %zu of %d pixels\n", bytes_read, SIZE);
                free(p6_thread);
                free(p6_simd);
                free(p6_simd_thread);
                fclose(fp);
                return -1;
        }

        fclose(fp);
        return 0;
}

void writeP6(const char *filename, struct Rgb *data) {
        FILE *fp = fopen(filename, "wb");
        if (!fp) {
                perror("Error opening file for writing");
                return;
        }

        fprintf(fp, "P6\n%d %d\n255\n", WIDTH, HEIGHT);

        fwrite(data, sizeof(struct Rgb), SIZE, fp);

        fclose(fp);
        printf("Written to %s\n", filename);
}

int main() {
        if (readP6("assignment.ppm") != 0) {
                printf("Failed to read PPM file. Exiting.\n");
                return 1;
        }

        printf("Original image loaded: %d x %d = %d pixels\n", WIDTH, HEIGHT, SIZE);

        memcpy(p6_simd, p6_thread, SIZE * sizeof(struct Rgb));
        memcpy(p6_simd_thread, p6_thread, SIZE * sizeof(struct Rgb));

        double thread_time = measure_threads(grayscale_thread_func);
        printf("Threads execution time: %.2f ms\n", thread_time);
        writeP6("assignment_threads_grayscale.ppm", p6_thread);

        double simd_time = measure_func_time(grayscale_simd_func);
        printf("Simd execution time: %.2f ms\n", simd_time);
        writeP6("assignment_simd.ppm", p6_simd);

        double thread_simd_time = measure_threads(grayscale_simd_thread_func);
        printf("Threads execution time: %.2f ms\n", thread_simd_time);
        writeP6("assignment_threads_simd_grayscale.ppm", p6_simd_thread);

        free(p6_thread);
        free(p6_simd);
        free(p6_simd_thread);

        return 0;
}

