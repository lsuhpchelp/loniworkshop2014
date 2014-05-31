#include <stdio.h>
#include <math.h>
#include <omp.h>
#include "timer.h"

int main(int argc, char** argv) {
    
    long long int i;
    const long long n=500000000;
    float a=2.0;

    float runtime;

    float x[n],y[n];

    for (i = 0; i < n; i++){
        x[i] = 1.0;
        y[i] = 2.0;
    }

    StartTimer();
#pragma acc kernels create(x[0:n],y[0:n])//parallel loop 
    for (i = 0; i < n; i++){
        y[i] = a*x[i]*x[i] + y[i]*y[i];
    }
    runtime = GetTimer();
    printf(" total acc time: %f sec\n", runtime/1000);

    StartTimer();
    for (i = 0; i < n; i++){
        y[i] = a*x[i]*x[i] + y[i]*y[i];
    }
    runtime = GetTimer();

    printf(" total serial time: %f sec\n", runtime/1000);

    return 0;
}
