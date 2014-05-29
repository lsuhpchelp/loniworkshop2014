#include <stdio.h>
#include <stdlib.h>
#include "timer.h" 

int main(int argc, char** argv) {
    long long int i, n=1000000000;
    if (argc>1) n=atoi(argv[1]);
    double x, pi;
    double sum = 0.0;
    double step = 1.0/(double) n; 
    printf("step = %17.15f\n",step);

    StartTimer();
#pragma acc kernels 
    for (i = 0; i < n; i++) {
        x = (i+0.5)*step;
        sum +=  4.0/(1.0+x*x);
    }
    pi = step * sum;
    double runtime=GetTimer();
    printf("pi = %17.15f, runtime = %f\n",pi, runtime/1000);
    return 0;
}
