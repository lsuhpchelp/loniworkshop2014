#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "dynamic_2d_array.h"             

int matmul_acc(/* FIXME: complete the declaration here */);

int main(int argc, char** argv) {
    // Exit if the number of arguments is not 6.
    if (argc != 6) {
        printf("Usage: mmacc nra nca ncb check eps\n");
        exit(1);
    }

    // Process the command line arguments.
    int nra=atoi(argv[1]);
    int nca=atoi(argv[2]);
    int ncb=atoi(argv[3]);
    int check=atoi(argv[4]);
    real eps=atof(argv[5]);

    // Get some timing information.
    int i,j,k;
    real **a, **b, **c;
    real **cs, **comp;
    real sum;
    real start_time, end_time;

    // FIXME: allocate the memory spaces from here:

    // initialzie the value
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
    matmul_acc(a, b, c, nra, nca, ncb);
    end_time = omp_get_wtime();
    printf (" total acc time: %f sec\n", end_time - start_time);

    // OpenMP versions
    int omp_threads=omp_get_num_procs();
    printf(" total num of procs: %d\n", omp_threads);
    int ii;
    const int num_omp_tests=3;
    int ompt[num_omp_tests];
    ompt[0]=omp_threads;
    ompt[1]=omp_threads/2;
    ompt[2]=omp_threads/4;
    // we will test "num_omp_tests" different number of omp threads
    for (ii=0;ii<num_omp_tests;ii++) {
        // get the time info for each omp run
        start_time = omp_get_wtime();
#pragma omp parallel shared(a,b,comp) private(i,j,k) num_threads(ompt[ii])
        {
#pragma omp for
            for (i = 0; i < nra; i++){
                for (k = 0; k < ncb; k++){
                    for (j = 0; j < nca; j++){
                        comp[i][k] += a[i][j] * b[j][k];
                    }
                }
            }
            omp_threads=omp_get_num_threads();
        }
        end_time = omp_get_wtime();
        printf (" total omp time with %d threads: %f sec\n", omp_threads, end_time - start_time);
    }

    if (check) {
        start_time = omp_get_wtime();
        for (i = 0; i < nra; i++){
            for (k = 0; k < ncb; k++){
                sum = 0.0;
                for (j = 0; j < nca; j++){
                    sum = sum + a[i][j] * b[j][k];
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
            printf("acc and serial do not match!\n");
        else
            printf("acc and serial match!\n");
    }        

    /* do not forget to free your memory */
}

// FIXME: complete the function implementation
int matmul_acc(/*complete the declaration here */){
    int i, j, k;
    real sum;
    // FIXME: complete the OpenACC directives and the code segments
    for (i = 0; i < nra; i++){
        for (k = 0; k < ncb; k++){
            sum = 0.0;
            for (j = 0; j < nca; j++){
            }
            c[i][k] = sum;
        }
    }
}

