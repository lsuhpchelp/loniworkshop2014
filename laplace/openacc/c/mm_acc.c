#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "dynamic_2d_array.h"             

int main(int argc, char** argv) {
    // printf("line number %d in file %s\n", __LINE__, __FILE__);
    // Exit if the number of arguments is not 5.
    if (argc != 4) {
        printf("Usage: mmacc nra nca ncb \n");
        exit(1);
    }

    // Process the command line arguments.

    int nra=atoi(argv[1]);
    int nca=atoi(argv[2]);
    int ncb=atoi(argv[3]);

    // Get some timing information.
    StartTimer();
    int i,j,k;
    // int nra=2, nca=3, ncb=2;
    //real a[nra][nca],b[nca][ncb],c[nra][ncb];
    real **restrict a, **restrict b, **restrict c;
    a=allocate_dynamic_2d_array(nra, nca);
    b=allocate_dynamic_2d_array(nca, ncb);
    c=allocate_dynamic_2d_array(nra, ncb);

    real sum;
    char fmt_string[]="%5.1f ";

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
        }
    }
//#pragma acc data create(a[0:nra][0:nca],b[0:nca][0:ncb]) copyout(c[0:nra][0:ncb])
#pragma acc data create(a[0:nra][0:nca],b[0:nca][0:ncb],c[0:nra][0:ncb])
    {

#pragma acc parallel loop 
        for (i = 0; i < nra; i++){
            for (j = 0; j < nca; j++){
                a[i][j] = (real)(i+j);
            }
        }

#pragma acc parallel loop 
        for (j = 0; j < nca; j++){
            for (k = 0; k < ncb; k++){
                b[j][k] = (real)(j*k);
            }
        }

#pragma acc parallel loop 
        for (i = 0; i < nra; i++){
            for (k = 0; k < ncb; k++){
                c[i][k] = 0.0;
            }
        }

#pragma acc kernels
        for (i = 0; i < nra; i++){
            for (k = 0; k < ncb; k++){
                sum = 0.0;
                for (j = 0; j < nca; j++){
                    sum = sum + a[i][j] * b[j][k];
                }
                c[i][k] = sum;
            }
        }
    }

    real runtime = GetTimer();
    printf(" total time: %f sec\n", runtime / 1000);
    ///* output A matrix*/
    //printf("\n A=\n");
    //print_matrix(a,nra,nca,fmt_string);

    ///* output B matrix*/
    //printf("\n B=\n");
    //print_matrix(b,nca,ncb,fmt_string);

    ///* output C matrix*/
    //printf("\n C=\n");
    //print_matrix(c,nra,nca,fmt_string);

    /* do not forget to free your memory */
    free_dynamic_2d_array(a);
    free_dynamic_2d_array(b);
    free_dynamic_2d_array(c);
}
