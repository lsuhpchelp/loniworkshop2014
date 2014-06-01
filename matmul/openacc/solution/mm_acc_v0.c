#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

typedef double real;

const int nra=2048;
const int nca=2048;
const int ncb=2048;

int main(int argc, char** argv) {

    int check=1;
    real eps=0.001;

    real start_time, end_time;

    // Get some timing information.
    int i,j,k;
    real a[nra][nca], b[nca][ncb], c[nra][ncb];
    real cs[nra][ncb];
    real sum;

    for (i = 0; i < nra; i++){
        for (j = 0; j < nca; j++){
            a[i][j] = (real)(i+j);
        }
    }

    for (j = 0; j < nca; j++){
        for (k = 0; k < ncb; k++){
            b[j][k] = (real)(j*k);
        }
    }

    for (i = 0; i < nra; i++){
        for (k = 0; k < ncb; k++){
            c[i][k] = 0.0;
            cs[i][k] = 0.0;
        }
    }

    // get the time info for the acc version
    start_time = omp_get_wtime();
    // FIXME: add the directives to the code segment below
#pragma acc data copyin(a,b) copyout(c)
#pragma acc kernels
    //#pragma acc parallel loop
    for (i = 0; i < nra; i++){
        //#pragma acc loop
        for (k = 0; k < ncb; k++){
            sum=0.0;
            //#pragma acc loop seq
            {
                for (j = 0; j < nca; j++){
                    sum += a[i][j] * b[j][k];
                }
                c[i][k] = sum;
            }
        }
    }
    end_time = omp_get_wtime();
    printf (" total acc time: %f sec\n", end_time - start_time);

    // check if the acc version matches the serial version
    if (check) {
        start_time = omp_get_wtime();
        for (i = 0; i < nra; i++){
            for (k = 0; k < ncb; k++){
                sum = 0.0;
                for (j = 0; j < nca; j++){
                    sum += a[i][j] * b[j][k];
                }
                cs[i][k] = sum;
            }
        }
        end_time = omp_get_wtime();
        printf (" total serial time: %f sec\n", end_time - start_time);

        int err = 0;
        // check the values of acc and serial versions
        for (i = 0; i < nra; i++){
            for (k = 0; k < ncb; k++){
                real diff=fabs(c[i][k]-cs[i][k]);
                if ( diff>eps ){ 
                    printf("error occurred at c[%d][%d]=%lf,cs[%d][%d]=%lf,diff=%lf\n",
                            i,k,c[i][k],i,k,cs[i][k],diff);
                    i=nra,k=ncb,err = 1;
                    break;
                }
            }
        }
        if (err) 
            printf("acc and serial does not match!\n");
        else
            printf("acc and serial matches!\n");
    }        

}
