#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

void saxpy(int n, float a, float *x, float *restrict y);

int main(int argc, char **argv) {
    int n = 1<<20; // 1 million floats

    if (argc > 1)
        n = atoi(argv[1]);

    float *x = (float*)malloc(n * sizeof(float));
    float *y = (float*)malloc(n * sizeof(float));

    for (int i = 0; i < n; ++i) {
        x[i] = 2.0f;
        y[i] = 1.0f;
    }

    StartTimer();
    saxpy(n, 3.0f, x, y);
    float runtime= GetTimer();
    printf(" total function time: %f sec\n", runtime/1000);

    free(x);
    free(y);

    return 0;
}

void saxpy(int n, float a, float *restrict x, float *restrict y) {
#pragma acc parallel loop
    for (int i = 0; i < n; ++i)
        y[i] = a * x[i] + y[i];
}

