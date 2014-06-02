#include <stdio.h> 
#include <stdlib.h> 
#include <omp.h> 
#include <stdint.h> 
#include <math.h> 

typedef double real;

int main(int argc, char** argv) { 
    long i, j, n;
    n=10000000000; //10^10 
    if (argc>1) n=atoi(argv[1]); 
    real start_time, end_time, run_time;
    real x, pi; 
    real sum = 0.0, sumA = 0.0; 
    real step; 
    step = 1.0/(real) n; 
    printf("step= %17.15f n= %ld\n",step,n); 
    n = (long) sqrtf((float)n); 

    start_time = omp_get_wtime();
#pragma acc kernels loop gang reduction(+:sum) 
    for (i = 0; i < n; i++) { 
        sumA = 0.0; 
#pragma acc loop vector reduction(+:sumA) 
        for (j = 0; j < n; j++) { 
            x = ((n*i)+j+0.5)*step; 
            sumA +=  4.0/(1.0+x*x); 
        } 
        sum += sumA; 
    } 
    pi = step * sum; 
    end_time = omp_get_wtime();
    run_time = end_time - start_time;
    printf(" pi acc = %17.15f\n",pi);
    printf(" total acc time: %g sec\n", run_time);
    return 0; 
} 
