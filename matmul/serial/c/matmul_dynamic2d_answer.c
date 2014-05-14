#include <stdio.h>
#include <stdlib.h>
#include "dynamic_2d_array.h"             

int main() {
    int i,j,k;
    int nra=2, nca=3, ncb=2;
    //real a[nra][nca],b[nca][ncb],c[nra][ncb];
    real **a, **b, **c;
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

    for (i = 0; i < nra; i++){
        for (k = 0; k < ncb; k++){
            sum = 0.0;
            for (j = 0; j < nca; j++){
                sum = sum + a[i][j] * b[j][k];
            }
            c[i][k] = sum;
        }
    }

    /* output A matrix*/
    printf("\n A=\n");
    print_matrix(a,nra,nca,fmt_string);

    /* output B matrix*/
    printf("\n B=\n");
    print_matrix(b,nca,ncb,fmt_string);

    /* output C matrix*/
    printf("\n C=\n");
    print_matrix(c,nra,nca,fmt_string);

    /* do not forget to free your memory */
    free_dynamic_2d_array(a);
    free_dynamic_2d_array(b);
    free_dynamic_2d_array(c);
}
