#include <stdio.h>
#include <stdlib.h>
             
typedef double real;

real **allocate_dynamic_2d_array(int nrows, int ncols);
void free_dynamic_2d_array(real** array_dynamic);
void print_matrix(real** array_dynamic, int nrows, int ncols, char* fmt_string);

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

real** allocate_dynamic_2d_array(int nrows, int ncols) {
    /* here is the method to correct the non-contiguous memory problem */
    int i;
    real** array_dynamic=(real**)malloc(nrows*sizeof(real*));
    array_dynamic[0]=(real*)malloc(nrows*ncols*sizeof(real));
    for (i=1; i<nrows; i++){
        array_dynamic[i]=array_dynamic[0]+ncols*i;
    }
    return array_dynamic;
}

void free_dynamic_2d_array(real** array_dynamic){
    free((void*)array_dynamic[0]);
    free((void*)array_dynamic);
}

void print_matrix(real** array_dynamic, int nrows, int ncols, char* fmt_string) {
    int i,j;
    for (i = 0; i < nrows; i++){
        for (j = 0; j < ncols; j++){
            printf(fmt_string, array_dynamic[i][j]);
        }
        printf("\n");
    }
}
