#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

#define DELIVERY_COUNT 10000

struct Order {
        int order_id;
        double distance_km;
        int priority; // 1 - HIGH, 0 - LOW
};
struct Order orders[DELIVERY_COUNT];

int main () {
        double threshold;
        int total_high_count = 0;
        int thread_high_count[4] = {0};

        double start, end;
        start = omp_get_wtime();

        #pragma omp parallel num_threads(4)
        {
        int id = omp_get_thread_num();

        #pragma omp single
        {
                threshold = 15.0;
        }

        unsigned int seed = time(NULL) ^ id;

        #pragma omp for
        for (int i = 0; i < DELIVERY_COUNT; i++) {
                orders[i].order_id = i;
                orders[i].distance_km = ((double)rand_r(&seed) / RAND_MAX) * 100.0;
        }

        #pragma omp for
        for (int i = 0; i < DELIVERY_COUNT; i++) {
                orders[i].priority = orders[i].distance_km < threshold;
        }

        #pragma omp single
        {
                printf("The priority assignment is finished!\n");
        }

        int local_count = 0;

        #pragma omp for
        for (int i = 0; i < DELIVERY_COUNT; i++) {
                if (orders[i].priority == 1) {
                        local_count++;
                }
        }

        thread_high_count[id] = local_count;
	#pragma omp barrier

        #pragma omp single
        {
                printf("High priority orders by chunks:\n");

                for (int i = 0; i < 4; i++) {
                printf("%d: %d\n", i, thread_high_count[i]);
                total_high_count += thread_high_count[i];
                }

                end = omp_get_wtime();

                printf("Total count: %d\nExecuted in %f seconds\n",
                total_high_count, end - start);
        }
        }

        return 0;
}
