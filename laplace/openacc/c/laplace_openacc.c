#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <time.h>
#include "timer.h"
#include "dynamic_2d_array.h"

#define MAXITER 1000000

int main(int argc, char* argv[])
{
    // printf("line number %d in file %s\n", __LINE__, __FILE__);
    // Exit if the number of arguments is not 5.
    if (argc != 6) {
        printf("Usage: laplace nrows ncols niter iprint relerr\n");
        exit(1);
    }

    // Process the command line arguments.

    int nr=atoi(argv[1]);
    int nc=atoi(argv[2]);
    int niter=atoi(argv[3]);
    int iprint=atoi(argv[4]);
    real relerr=atof(argv[5]);

    int i,j,iter;
    real dt;

    // Get some timing information.
    //clock_t time1=clock();
    StartTimer();

    int nr2 = nr+2;
    int nc2 = nc+2;

    real **restrict t   =allocate_dynamic_2d_array(nr2,nc2);
    real **restrict told=allocate_dynamic_2d_array(nr2,nc2);

    // Initialize the array.
    for (i=0;i<nr2;i++)
        for (j=0;j<nc2;j++) {
            t[i][j]=0.0;
            told[i][j]=0.0;
        }

    // Set the boundary condition.
    // Right boundary
    for (i=0;i<nr2;i++) {
        t[i][nc+1]=(100.0/nr)*i;
        told[i][nc+1]=t[i][nc+1];
    }
    // Bottom boundary
    for (j=0;j<nc2;j++) {
        t[nr+1][j]=(100.0/nc)*j;
        told[nr+1][j]=t[nr+1][j];
    }
    //printf("line number %d in file %s\n", __LINE__, __FILE__);

    // Main loop.
#pragma acc data copy(told[0:nr2][0:nc2]), create(t[0:nr2][0:nc2])
    for (iter=1;iter<=niter;iter++) {

//#pragma acc parallel loop
#pragma acc kernels
        for (i=1;i<=nr;i++)
            for (j=1;j<=nc;j++)
                t[i][j]=0.25*(told[i+1][j]+told[i-1][j]+told[i][j-1]+told[i][j+1]);
        // Check on convergence, and move current values to old
        dt=0;

//#pragma acc parallel loop
#pragma acc kernels
        for (i=1;i<=nr;i++) {
            for (j=1;j<=nc;j++) {
                dt=fmax(fabs(t[i][j]-told[i][j]),dt);
                told[i][j]=t[i][j];
            }
        }

        // Check if output is required.
        if (iprint != 0)
            if (iter%iprint == 0) 
                printf("Iteration: %d; Convergence Error: %f\n",iter,dt);

        // Check if convergence criteria meet.
        if (dt < relerr) {
            printf("\nSolution has converged.\n");
            break;
        }
    } // end #pragma acc parallel

    free_dynamic_2d_array(t);
    free_dynamic_2d_array(told);

    // Print out the execution time.
    real runtime = GetTimer();
    printf(" total time: %f sec\n", runtime / 1000);

    return 0;

}
