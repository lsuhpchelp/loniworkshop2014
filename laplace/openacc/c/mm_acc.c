#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "timer.h"
#include "dynamic_2d_array.h"             

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
    real **restrict a, **restrict b, **restrict c;
    real **cs, **comp;
    real sum;

    a=allocate_dynamic_2d_array(nra, nca);
    b=allocate_dynamic_2d_array(nca, ncb);
    c=allocate_dynamic_2d_array(nra, ncb);
    cs=allocate_dynamic_2d_array(nra, ncb);
    comp=allocate_dynamic_2d_array(nra, ncb);

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
    StartTimer();
#pragma acc data copyin(a[0:nra][0:nca],b[0:nca][0:ncb]) copy(c[0:nra][0:ncb])
//#pragma acc data create(a[0:nra][0:nca],b[0:nca][0:ncb],c[0:nra][0:ncb])
    {
#pragma acc kernels
        for (i = 0; i < nra; i++){
            for (k = 0; k < ncb; k++){
                sum = 0.0;
                for (j = 0; j < nca; j++){
                    sum += a[i][j] * b[j][k];
                }
                c[i][k] = sum;
            }
        }
    }
    real runtime = GetTimer();
    printf(" total acc time: %f sec\n", runtime / 1000);

    // get the time info for the omp version
    
    int omp_threads=omp_get_num_procs();
    int ii;
    const int num_omp_tests=5;
    int ompt[num_omp_tests];
    ompt[0]=omp_threads*4;
    ompt[1]=omp_threads*2;
    ompt[2]=omp_threads;
    ompt[3]=omp_threads/2;
    ompt[4]=omp_threads/4;
    // we will test 4 different number of omp threads
    for (ii=0;ii<num_omp_tests;ii++) {
        StartTimer();
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
        runtime = GetTimer();
        printf(" total omp time: %f sec, using %d threads\n", runtime / 1000,omp_threads);
    }

    if (check) {
        StartTimer();
        for (i = 0; i < nra; i++){
            for (k = 0; k < ncb; k++){
                sum = 0.0;
                for (j = 0; j < nca; j++){
                    sum = sum + a[i][j] * b[j][k];
                }
                cs[i][k] = sum;
            }
        }

        runtime = GetTimer();
        printf(" total serial time: %f sec\n", runtime / 1000);

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

    /* do not forget to free your memory */
    free_dynamic_2d_array(a);
    free_dynamic_2d_array(b);
    free_dynamic_2d_array(c);
    free_dynamic_2d_array(cs);
}
