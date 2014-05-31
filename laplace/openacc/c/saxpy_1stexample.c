#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "timer.h"

void saxpy(long n, float a, float *restrict x, float *restrict y);

int main(int argc, char **argv) {
    //int n = 1<<20; // 1 million floats
    long n = 500000000;
    //long n = 5;
    double start_time, end_time;

    if (argc > 1)
        n = atoi(argv[1]);

    float *restrict x = (float*)malloc(n * sizeof(float));
    float *restrict y = (float*)malloc(n * sizeof(float));
    float a=3.0f;

    StartTimer();
#pragma acc kernels create(x[0:n]) copyout(y[0:n])
    {
        for (int i = 0; i < n; ++i) {
            x[i] = 2.0f;
            y[i] = 1.0f;
        }

        for (int i = 0; i < n; ++i)
            y[i] = a * x[i] + y[i];
        //saxpy(n, 3.0f, x, y);
    }
    double runtime = GetTimer();
    printf(" total function time: %f sec\n", runtime/1000);
    printf("y[0]=%f\n",y[0]);

    free(x);
    free(y);

    return 0;
}

void saxpy(long n, float a, float *restrict x, float *restrict y) {
    //#pragma acc kernels
    for (long i = 0; i < n; ++i)
        y[i] = a * x[i] + y[i];
}

