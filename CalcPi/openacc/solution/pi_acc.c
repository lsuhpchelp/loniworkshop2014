#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef double real;

int main() {
    long long i, n=1000000000;  // 10^9
    printf("n=%ld\n",n);
    real x, pi;
    real sum = 0.0;
    real step = 1.0/(real) n; 
    real start_time, end_time, run_time;

#pragma acc data copyin(step) 
    {
        start_time = omp_get_wtime();
//FIXME: complete the OpenACC directive here:
#pragma acc parallel loop reduction(+:sum)
//#pragma acc kernels
        for (i = 0; i < n; i++) {
        // FIXME complete the iteration loop here f(x) = 4.0/(1+x*x)
            x = (i+0.5)*step;
            sum += 4.0/(1.0+x*x);
        }
        pi = step * sum;
        end_time = omp_get_wtime();
    }
    run_time = end_time - start_time;
    printf(" pi acc = %17.15f\n",pi);
    printf(" total acc time: %g sec\n", run_time);

    // omp run
    int omp_threads;
    //int omp_threads=omp_get_num_procs();
    sum = 0.0;
    start_time = omp_get_wtime();
#pragma omp parallel private(i,x) reduction(+:sum) //num_threads(omp_threads)
    {
#pragma omp for
        for (i = 0; i < n; i++) {
            x = (i+0.5)*step;
            sum += 4.0/(1.0+x*x);
        }
        omp_threads=omp_get_num_threads();
    }
    pi = step * sum;
    end_time = omp_get_wtime();
    run_time = end_time - start_time;
    printf(" pi omp = %17.15f\n",pi);
    printf(" total omp time with %d threads: %f sec\n", omp_threads, run_time);

    // serial run
    sum = 0.0;
    start_time = omp_get_wtime();
    for (i = 0; i < n; i++) {
        x = (i+0.5)*step;
        sum += 4.0/(1.0+x*x);
    }
    pi = step * sum;
    end_time = omp_get_wtime();
    run_time = end_time - start_time;
    printf("pi serial = %17.15f\n",pi);
    printf(" total serial time: %f sec\n", run_time);

    return 0;

}
