#include <stdio.h>
#include <math.h>
#include "timer.h"

typedef float real;

int main() {
    long long int i, n=500000000;
    float a=2.0;
    real *restrict x;
    real *restrict y;
    real runtime;

    x = (real*)malloc(n*sizeof(real));
    y = (real*)malloc(n*sizeof(real));

    for (i = 0; i < n; i++){
        x[i] = 1.0;
        y[i] = 2.0;
    }

    StartTimer();
#pragma acc parallel loop private(i), firstprivate(a)
    for (i = 0; i < n; i++){
        y[i] = a*x[i] + y[i];
    }
    runtime = GetTimer();
    printf(" total acc time: %f sec\n", runtime/1000);

    free(x);
    free(y);

    return 0;
}
