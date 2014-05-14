#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "timer.h"
#include "../../serial/dynamic_2d_array.h"             

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
            comp[i][k] = 0.0;
        }
    }

    // get the time info for the omp version
    StartTimer();
    //#pragma omp parallel for default(none) shared(a[nra][nca],b[nca][ncb],c[nra][ncb]) private(i,j,k)
#pragma omp parallel for shared(a,b,comp) private(i,j,k)
    for (i = 0; i < nra; i++){
        for (k = 0; k < ncb; k++){
            for (j = 0; j < nca; j++){
                comp[i][k] += a[i][j] * b[j][k];
            }
        }
    }
    real runtime = GetTimer();
    printf(" total omp time: %f sec\n", runtime / 1000);

    /* do not forget to free your memory */
    free_dynamic_2d_array(a);
    free_dynamic_2d_array(b);
    free_dynamic_2d_array(c);
    free_dynamic_2d_array(comp);
}
