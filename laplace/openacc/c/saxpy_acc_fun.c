#include <stdio.h>
#include <math.h>
#include "timer.h"

typedef float real;

int main(int argc, char** argv) {
    
    long long int i, n;
    float a=2.0;

    real *x;
    real *y;
    real runtime;

    n=atoi(argv[1]);

    x = (real*)malloc(n*sizeof(real));
    y = (real*)malloc(n*sizeof(real));

    for (i = 0; i < n; i++){
        x[i] = 1.0;
        y[i] = 2.0;
    }

    StartTimer();
//#pragma acc kernels
#pragma acc parallel loop
    for (i = 0; i < n; i++){
        y[i] = a*x[i] + y[i];
    }
    runtime = GetTimer();
    printf(" total acc time: %f sec\n", runtime/1000);

    free(x);
    free(y);

    return 0;
}
