// http://www.drdobbs.com/parallel/creating-and-using-libraries-with-openac/240012502?pgno=2
/* simpleMult.c for OpenACC and OpenMP */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

//    A simple square matrix multiply using conditional data clauses
void doMult(int size, float (* restrict A)[size], float (* restrict B)[size],
        float (* restrict C)[size]) 
{
    // Compute matrix multiplication.
#pragma omp parallel for default(none) shared(A,B,C,size)
#pragma acc kernels pcopyin(A[0:size][0:size],B[0:size][0:size]) \
    pcopyout(C[0:size][0:size])

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            float tmp = 0.;
            for (int k = 0; k < size; ++k) {
                tmp += A[i][k] * B[k][j];
            }
            C[i][j] = tmp;
        }
    }
}

void fill(int size, float (* restrict A)[size], float (* restrict B)[size]) 
{
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            A[i][j] = ((float)i + j); B[i][j] = ((float)i - j);
        }
    }
}

int main(int argc, char *argv[])
{

    if(argc != 3) {
        fprintf(stderr,"Use: %s size nIter\n",argv[0]);
        return -1;
    }

    int size=atoi(argv[1]);
    int nIter=atoi(argv[2]);

    if(nIter <= 0) {
        fprintf(stderr,"%s: Invalid nIter (%d)\n",argv[0],nIter);
        return -1;
    }

    // allocate the square matrices
    float (*restrict A)[size] = malloc(sizeof(float)*size*size);
    float (*restrict B)[size] = malloc(sizeof(float)*size*size);
    float (*restrict C)[size] = malloc(sizeof(float)*size*size);

    fill(size,A,B);

    // Ensure the A and B matrices are present on the device  
#pragma acc data pcopyin(A[0:size][0:size],B[0:size][0:size])
    for(int i=0; i < nIter; i++) {
        double startTime = omp_get_wtime();
        doMult(size,A,B,C);
        double endTime = omp_get_wtime();
        printf("%s runtime %8.5g\n",argv[0], (endTime-startTime));
    }

    free(A); free(B); free(C);

    return 0;
}
