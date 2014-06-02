#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void saxpy_acc(long n, float a, float *x, float *restrict y, float xval, float yval);
void saxpy_omp(long n, float a, float *x, float *restrict y, float xval, float yval);
void saxpy_serial(long n, float a, float *x, float *restrict y, float xval, float yval);

int main(int argc, char **argv) {
    long n = 500000000;
    if (argc > 1) n = atoi(argv[1]);

    float *x = (float*)malloc(n * sizeof(float));
    float *y = (float*)malloc(n * sizeof(float));
    float a=3.0f, xv=2.0f, yv=1.0f;

    saxpy_acc(n, a, x, y, xv, yv);
    saxpy_omp(n, a, x, y, xv, yv);
    saxpy_serial(n, a, x, y, xv, yv);

    free(x);
    free(y);

    return 0;
}

void saxpy_acc(long n, float a, float *x, float *restrict y, float xval, float yval) {
    double start_time, end_time;
    //FIXME: complete the OpenACC directives here
    {
        for (int i = 0; i < n; ++i) {
            x[i] = xval;
            y[i] = yval;
        }
        start_time = omp_get_wtime();
        for (int i = 0; i < n; ++i)
            y[i] = a * x[i] + y[i];
        end_time = omp_get_wtime();
        printf ("SAXPY acc Time: %f\n", end_time - start_time);
    }
}

void saxpy_serial(long n, float a, float *x, float *restrict y, float xval, float yval) {
    double start_time, end_time;
    for (int i = 0; i < n; ++i) {
        x[i] = 2.0f;
        y[i] = 1.0f;
    }
    start_time = omp_get_wtime();
    for (int i = 0; i < n; ++i)
        y[i] = a * x[i] + y[i];
    end_time = omp_get_wtime();
    printf ("SAXPY serial Time: %f\n", end_time - start_time);
}

void saxpy_omp(long n, float a, float *x, float *restrict y, float xval, float yval) {
    double start_time, end_time;
    for (int i = 0; i < n; ++i) {
        x[i] = 2.0f;
        y[i] = 1.0f;
    }
    start_time = omp_get_wtime();
#pragma omp parallel for 
    for (int i = 0; i < n; ++i)
        y[i] = a * x[i] + y[i];
    end_time = omp_get_wtime();
    printf ("SAXPY omp Time: %f\n", end_time - start_time);
}
