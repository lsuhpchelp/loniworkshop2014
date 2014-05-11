#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAXITER 1000000

int main(int argc, char* argv[])
{
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
    double relerr=atof(argv[5]);

    int i,j,iter;
    double dt;

    // Get some timing information.
    clock_t time1=clock();

    double **t;
    t=malloc((nr+2)*sizeof(double *));
    for (i=0;i<nr+2;i++)
        t[i]=malloc((nc+2)*sizeof(double *));

    double **told;
    told=malloc((nr+2)*sizeof(double *));
    for (i=0;i<nr+2;i++)
        told[i]=malloc((nc+2)*sizeof(double *));

    // Initialize the array.                                                                                                                                                                           
    for (i=0; i<nr+2; i++)
        for (j=0; j<nc+2; j++)
            told[i][j]=0.;

    // Set the boundary condition.

    // Right boundary
    for (i=0;i<nr+2;i++) {
        t[i][nc+1]=(100.0/nr)*i;
        told[i][nc+1]=t[i][nc+1];
    }

    // Bottom boundary
    for (j=0;j<nc+2;j++) {
        t[nr+1][j]=(100.0/nc)*j;
        told[nr+1][j]=t[nr+1][j];
    }

    // Main loop.

    for (iter=1;iter<=niter;iter++) {

        for (i=1;i<=nr;i++)
            for (j=1;j<=nc;j++)
                t[i][j]=0.25*(told[i+1][j]+told[i-1][j]+told[i][j-1]+told[i][j+1]);

        // Check on convergence, and move current values to old

        dt=0;
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
    }

    free(t);
    free(told);

    // Print out the execution time.
    clock_t time2=clock()-time1;
    int msec=time2/CLOCKS_PER_SEC;
    printf("\nTotal Time (sec): %d.\n",msec);

    return 0;

}
