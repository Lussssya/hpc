#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

#define N 100000000
#define RANGE 256

int naive_hist[RANGE] = {0};
int crit_hist[RANGE] = {0};
int red_hist[RANGE] = {0};
int A[N];

double naive_parallel () {
        double start, end;
        start = omp_get_wtime();

        #pragma omp parallel num_threads(8)
        {
        #pragma omp for
        for (int i = 0; i < N; i++) {
                naive_hist[A[i]]++;
        }
        }

        end = omp_get_wtime();
        return end - start;
}

double critical_parallel () {
        double start, end;
        start = omp_get_wtime();

        #pragma omp parallel num_threads(8)
        {
        #pragma omp for
        for (int i = 0; i < N; i++) {
                #pragma omp critical
                {
                        crit_hist[A[i]]++;
                }
        }
        }

        end = omp_get_wtime();
        return end - start;
}

double reduction_parallel () {
        double start, end;
        start = omp_get_wtime();

        #pragma omp parallel num_threads(8)
        {
        #pragma omp for reduction (+ : red_hist[:RANGE])
        for (int i = 0; i < N; i++) {
                red_hist[A[i]]++;
        }
        }

        end = omp_get_wtime();
        return end - start;
}

int main () {
        srand(time(NULL));
        for (int i = 0; i < N; i++) {
                A[i] = rand() % 256;
        }
        double naive_parallel_exe_time = naive_parallel();
        double critical_parallel_exe_time = critical_parallel();
        double reduction_parallel_exe_time = reduction_parallel();
	int sum_naive = 0, sum_crit = 0, sum_red = 0;
	for (int i = 0; i < RANGE; i++) {
        	sum_naive += naive_hist[i];
        	sum_crit += crit_hist[i];
        	sum_red += red_hist[i];
        }

        printf("Naive approach execution time: %f\nsum: %d\n", naive_parallel_exe_time, sum_naive);
        printf("Critical section approach execution time: %f\nsum: %d\n", critical_parallel_exe_time, sum_crit);
        printf("Reduction approach execution time: %f\nsum: %d\n", reduction_parallel_exe_time, sum_red);

        return 0;
}
