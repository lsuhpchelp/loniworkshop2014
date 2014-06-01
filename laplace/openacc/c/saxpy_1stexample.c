#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "timer.h"

void saxpy(long n, float a, float *x, float *restrict y, float xval, float yval);

int main(int argc, char **argv) {
    long n = 500000000;
    double start_time, end_time;

    if (argc > 1)
        n = atoi(argv[1]);

    float *x = (float*)malloc(n * sizeof(float));
    float *y = (float*)malloc(n * sizeof(float));
    float a=3.0f;

    StartTimer();
    for (int i = 0; i < n; ++i) {
        x[i] = 2.0f;
        y[i] = 1.0f;
    }

    start_time = omp_get_wtime();
    for (int i = 0; i < n; ++i)
        y[i] = a * x[i] + y[i];
    end_time = omp_get_wtime();
    printf ("SAXPY omp serial Time: %f\n", end_time - start_time);
    double runtime = GetTimer();
    printf(" total serial time: %f sec\n", runtime/1000);

    StartTimer();
    saxpy(n, 3.0f, x, y, 2.0f, 1.0f);
    runtime = GetTimer();
    printf(" total function time: %f sec\n", runtime/1000);
    printf("y[0]=%f\n",y[0]);

    free(x);
    free(y);

    return 0;
}

void saxpy(long n, float a, float *x, float *restrict y, float xval, float yval) {
#pragma acc create(x[0:n],y[0:n])
    {
        double start_time, end_time;
        start_time = omp_get_wtime();
#pragma acc kernels loop
        for (int i = 0; i < n; ++i) {
            x[i] = xval;
            y[i] = yval;
        }
        double runtime = GetTimer();
#pragma acc kernels loop
        for (int i = 0; i < n; ++i)
            y[i] = a * x[i] + y[i];
        end_time = omp_get_wtime();
        printf ("SAXPY omp Time: %f\n", end_time - start_time);
        printf(" total saxpy time: %f sec\n", runtime/1000);
    }
}
