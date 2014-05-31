#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

int main(int argc, char** argv)
{
    //float *restrict a, *b, *c;
    long long int i;
    const long long n=1000000;
    float a[n],b[n],c[n];
    //a = (float*)malloc(n*sizeof(float));
    //b = (float*)malloc(n*sizeof(float));
    //c = (float*)malloc(n*sizeof(float));

    for( i = 0; i < n; ++i )
        a[i]=1.0,b[i]=2.0,c[i]=3.0;

    StartTimer();
//#pragma acc parallel loop create(a[0:n], b[0:n], c[0:n])
    #pragma acc kernels
    {
        #pragma acc loop
        for( i = 0; i < n; ++i ){
            a[i]=1.0;
            b[i]=2.0;
            c[i]=3.0;
        }
        #pragma acc loop
        for( i = 0; i < n; ++i )
            a[i] = b[i] + c[i];
    }
    double t = GetTimer();
    printf("line %d = %f\n",__LINE__, t/1000);

    StartTimer();
    //#pragma acc kernels //loop independent
    for( i = 0; i < n; ++i )
        a[i] = b[i] + c[i];
    t = GetTimer();
    printf("line %d = %f\n",__LINE__, t/1000);

    //free(a);
    //free(b);
    //free(c);
}
