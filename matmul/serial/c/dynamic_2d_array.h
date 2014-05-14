#include <stdio.h>
#include <stdlib.h>
             
typedef double real;

real **allocate_dynamic_2d_array(int nrows, int ncols);
void free_dynamic_2d_array(real** array_dynamic);
void print_matrix(real** array_dynamic, int nrows, int ncols, char* fmt_string);

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
