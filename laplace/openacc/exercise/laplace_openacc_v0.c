/*
 * from cudacast
 */

#include <math.h>
#include <string.h>
#include <omp.h>

#define NN 4096
#define NM 4096

typedef double real;

int main(int argc, char** argv)
{
    const int n = NN;
    const int m = NM;
    const int iter_max = 1000;

    const real tol = 1.0e-6;
    real error     = 1.0;
    real start_time, end_time;

    real A[n][m];
    real Anew[n][m];

    for (int i=0; i<n; i++)
        for (int j=0; j<m; j++) {
            A[i][j]=0.0;
            Anew[i][j]=0.0;
        }

    for (int j = 0; j < n; j++) {
        A[j][0]    = 1.0;
        Anew[j][0] = 1.0;
    }

    printf("Jacobi relaxation Calculation: %d x %d mesh\n", n, m);

    int iter = 0;

    start_time = omp_get_wtime();
//FIXME: add the directive for data region
    while ( error > tol && iter < iter_max )
    {
        error = 0.0;
//FIXME: add the directive for loops
        for( int j = 1; j < n-1; j++) {
            for( int i = 1; i < m-1; i++ ) {
                Anew[j][i] = 0.25 * ( A[j][i+1] + A[j][i-1]
                        + A[j-1][i] + A[j+1][i]);
                error = fmax( error, fabs(Anew[j][i] - A[j][i]));
            }
        }

//FIXME: add the directive for loops
        for( int j = 1; j < n-1; j++) {
            for( int i = 1; i < m-1; i++ ) {
                A[j][i] = Anew[j][i];    
            }
        }

        if(iter % 100 == 0) printf("%5d, %0.6f\n", iter, error);

        iter++;
    }
    end_time = omp_get_wtime();
    printf ("total time in sec: %f\n", end_time - start_time);
}
