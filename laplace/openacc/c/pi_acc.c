#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include <omp.h>

typedef double real;

int main() {
    int i;
    long n=1<<30;
    //n=10000000000;
    printf("n=%d\n",n);
    real x, pi;
    real sum = 0.0;
    real step = 1.0/(real) n; 

    StartTimer();
#pragma acc parallel loops private(i,x) reduction(+:sum)
    for (i = 0; i < n; i++) {
        x = (i+0.5)*step;
        sum += 4.0/(1.0+x*x);
    }
    pi = step * sum;
    real runtime = GetTimer();
    printf(" total time: %f sec\n", runtime / 1000);

    // omp run
    int omp_threads=omp_get_num_procs();
    sum = 0.0;
    StartTimer();
#pragma omp parallel private(i,x) reduction(+:sum) num_threads(omp_threads)
    {
#pragma omp for
        for (i = 0; i < n; i++) {
            x = (i+0.5)*step;
            sum += 4.0/(1.0+x*x);
        }
        omp_threads=omp_get_num_threads();
    }
    pi = step * sum;
    runtime = GetTimer();
    printf(" total time: %f sec, using %d omp threads.\n", runtime / 1000, omp_threads);

    // serial run
    sum = 0.0;
    StartTimer();
    for (i = 0; i < n; i++) {
        x = (i+0.5)*step;
        sum += 4.0/(1.0+x*x);
    }
    pi = step * sum;
    runtime = GetTimer();
    printf(" total time: %f sec\n", runtime / 1000);

    printf("pi = %17.15f\n",pi);
    return 0;
}
