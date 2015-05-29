#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void saxpy(int n, float a, float *x, float *restrict y) {
#pragma acc kernels
    for (int i = 0; i < n; ++i)
        y[i] = a*x[i] + y[i];
}

int main(int argc, char **argv)
{
    int n = 1<<20; // 1 million floats

    float *x = (float*)malloc(n*sizeof(float));
    float *y = (float*)malloc(n*sizeof(float));
    for (int i = 0; i < n; ++i) {
        x[i] = 2.0f;
        y[i] = 1.0f;
    }
    saxpy(n, 3.0f, x, y);
    printf("y[0]=%5.7f\n",y[0]);
    free(x);
    free(y);
    return 0;
}

