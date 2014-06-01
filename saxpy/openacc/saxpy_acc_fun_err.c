#include <stdio.h>
#include <math.h>
#include "timer.h"

typedef float real;

void saxpy(int n, float a, float *x, float *y);

int main(int argc, char** argv) {

    long long int i, n;
    float a=2.0;

    real *x;
    real *y;
    real runtime;
    n=500000000;
    //n=1<<28;
    if (argc>1)
        n=atoi(argv[1]);
    printf("%d elements.\n", n);

    x = (real*)malloc(n*sizeof(real));
    y = (real*)malloc(n*sizeof(real));

    for (i = 0; i < n; i++){
        x[i] = 1.0;
        y[i] = 2.0;
    }

    StartTimer();
#pragma omp parallel for
#pragma acc kernels
    //#pragma acc parallel loop
    for (i = 0; i < n; i++){
        y[i] = a*x[i] + y[i];
    }
    runtime = GetTimer();
    printf(" total inline time: %f sec\n", runtime/1000);

    StartTimer();
    saxpy( n, 2.0, x, y);
    runtime = GetTimer();
    printf(" total function time: %f sec\n", runtime/1000);

    free(x);
    free(y);

    return 0;
}

void saxpy(int n, float a, float *restrict x, float *restrict y) {
    int i;
#pragma omp parallel for
#pragma acc kernels
    //#pragma acc parallel loop
    for (i = 0; i < n; i++){
        y[i] = a*x[i] + y[i];
    }
}
