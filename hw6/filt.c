#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define N 500000

int A[N];
long long sum_elements;

double compute_sum () {
        int T;
        int max_element = INT_MIN;
        sum_elements = 0;

        double start, end;
        start = omp_get_wtime();

        #pragma omp parallel num_threads(8)
        {
        #pragma omp for reduction (max : max_element)
        for (int i = 0; i < N; i++) {
                if (A[i] > max_element) {
                        max_element = A[i];
                }
        }

        #pragma omp single
        {
                T = (int) 0.8 * max_element;
        }

        #pragma omp for reduction (+ : sum_elements)
        for (int i = 0; i < N; i++) {
                if (A[i] > T) {
                        sum_elements += A[i];
                }
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

        double execution_time = compute_sum();
        printf("Summary of array with treshold is: %lld\nexecution time: %f\n", sum_elements, execution_time);
        return 0;
}
