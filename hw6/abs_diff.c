#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>

#define N 50000000
#define UPPER_LIMIT 1000000

double A[N];
double min_abs_diff;

double find_minimum_absolute_difference () {
        double start, end;
        start = omp_get_wtime();

        min_abs_diff = DBL_MAX;

        #pragma omp parallel num_threads(8)
        {
        #pragma omp for reduction(min : min_abs_diff)
        for (int i = 1; i < N; i++) {
                double current_diff = fabs(A[i] - A[i - 1]);
		if (current_diff < min_abs_diff) {
                        min_abs_diff = current_diff;
                }
        }
        }

        end = omp_get_wtime();
        return end - start;
}

int main () {
        srand(time(NULL));
        for (int i = 0; i < N; i++) {
                A[i] = ((double)rand() / RAND_MAX) * UPPER_LIMIT;
        }

        double execution_time = find_minimum_absolute_difference();
        printf("Minimum absolute difference in the array is: %f\nexecution time: %f\n", min_abs_diff, execution_time);

        return 0;
}
