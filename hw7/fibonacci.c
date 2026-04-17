#include <stdio.h>
#include <omp.h>


long long recursive_fibonacci (int num) {
        if (num == 1 || num == 2) {
                return 1;
        }
        return recursive_fibonacci(num - 1) + recursive_fibonacci(num - 2);
}

long long fib_task (int num) {
        if (num <= 30) {
                return recursive_fibonacci(num);
        }

        long long prev, prePrev;

        #pragma omp task shared(prev)
        prev = fib_task(num - 1);

        #pragma omp task shared(prePrev)
        prePrev = fib_task(num - 2);

        #pragma omp taskwait

        return prev + prePrev;
}

long long parallel_fibonacci (int num) {
        long long result;

        #pragma omp parallel
        {
                #pragma omp single
                result = fib_task(num);
        }

        return result;
}

int main () {
        int num;
        printf("Enter an integer: ");
        scanf("%d", &num);

        double start_rec, end_rec;
        start_rec = omp_get_wtime();
        long long recursive_fib = recursive_fibonacci(num);
        end_rec = omp_get_wtime();
        printf("Recursive version.\nResult: %llu\nExecution time: %f\n", recursive_fib, end_rec - start_rec);

        double start_par, end_par;
        start_par = omp_get_wtime();
        //long long parallel_fib = parallel_fibonacci(num);
        end_par = omp_get_wtime();
        //printf("Parallel version.\nResult: %llu\nExecution time: %f\n", parallel_fib, end_par - start_par);

        return 0;
}

