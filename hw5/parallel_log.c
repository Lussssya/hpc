#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>

#define LOG_ENTRIES 20000

struct log_entry {
	int request_id;
	int user_id;
	double response_time_ms;
};
struct log_entry entries[LOG_ENTRIES];

// 0 - SLOW, 1 - MID, 2 - FAST
int classify[3] = {0};

int main () {
	unsigned int seed = (unsigned int)time(NULL) ^ omp_get_thread_num();
	double start, end;
	start = omp_get_wtime();

	#pragma omp parallel num_threads(4)
	{
	#pragma omp single
	{
		for (int i = 0; i < LOG_ENTRIES; i++) {
			entries[i].request_id = i;
			entries[i].user_id = i;
			entries[i].response_time_ms = ((double)rand_r(&seed) / RAND_MAX) * 1000.0;
		}
	}
	
	int classify_local[3] = {0};

	#pragma omp for
	for (int i = 0; i < LOG_ENTRIES; i++) {
		if (entries[i].response_time_ms < 100) {
			classify_local[2]++;
		} else if (entries[i].response_time_ms > 300) {
			classify_local[0]++;
		} else {
			classify_local[1]++;
		}
	}

	#pragma omp critical
	{
		for (int i = 0; i < 3; i++) {
			classify[i] += classify_local[i];
		}
	}
	}
	end = omp_get_wtime();


	printf("Histogram of log entries speed:\nSLOW: %d\nMEDIUM: %d\nFAST: %d\n", classify[0], classify[1], classify[2]);
	printf("Execution time: %f\n", end - start);
	return 0;
}
