#include <stdio.h>
#include <stdlib.h>
#include "timer.h" 

int main(int argc, char** argv) {
    long long int i, n=10000000000;
    if (argc>1) n=atoi(argv[1]);
    double x, pi;
    double sum = 0.0;
    double step = 1.0/(double) n; 
    printf("step = %17.15f\n",step);

    StartTimer();
#pragma acc kernels 
    for (i = 0; i < n; i++) {
        x = (i+0.5)*step;
        sum +=  4.0/(1.0+x*x);
    }
    pi = step * sum;
    double runtime=GetTimer();
    printf("pi acc = %17.15f, runtime = %f\n",pi, runtime/1000);
    //
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
    printf("pi omp %d = %17.15f, runtime = %f\n", omp_threads, pi, runtime/1000);
    return 0;
}
