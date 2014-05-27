
/******************************************************************************
* FILE: openmp-mm.c
*
* DESCRIPTION:  
*   In this example code, the matrix multiplication operation is explicitly
*   parallelized by specifying OpenMP thread construct on main computation loop.
*   The number of threads depends on OMP_NUM_THREADS environment variable.
*
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#define NRA 1500             /* Number of rows in matrix A */
#define NCA 2000             /* Number of columns (rows) in matrix A (B) */
#define NCB 1000             /* number of columns in matrix B */

#define dt(start, end)  ((end.tv_sec - start.tv_sec) + \
		      1/1000000.0*(end.tv_usec - start.tv_usec))

double a[NRA][NCA],          /* A - Multiplicand matrix */
       b[NCA][NCB],          /* B - Multiplier matrix */
       c[NRA][NCB];          /* C - Prodcut matrix */

main(){

	int i,j,k;
	struct timeval srun, scalc, ecalc;
	
	/* Start recording matrix initialization time (Init) */
	gettimeofday(&srun, NULL);

	/* Initialize the multiplicand matrix */
	for (i=0; i<NRA; i++) {
		for (j=0; j<NCA; j++) {
			a[i][j] = (double)(i+j);
		}
	}

	/* Initialize the multiplier matrix */
	for (i=0; i<NCA; i++) {
		for (j=0; j<NCB; j++) {
			b[i][j] = (double)(i*j);
		}
	}

	/* Start recording matrix multiplication (computation) time (Calc) */
	gettimeofday(&scalc, NULL);


	/* Create OpenMP threads and multiply matrix A and B */
	float sum;
#pragma omp parallel for private(sum,i,k,j)
	for (i=0; i<NRA; i++) {
		for (k=0; k<NCB; k++) {
			sum = 0.0;
			for (j=0; j<NCA; j++) {
				sum = sum + a[i][j] * b[j][k];
			}
			c[i][k] = sum;
		}
	}

	/* Stop recording computation time */
	gettimeofday(&ecalc, NULL);

	/* Print timing results */
	printf("OPENMP-MM: Init time: %3.02f   Calc time: %3.02f   GFlop/s: %3.02f\n",
		dt(srun, scalc), dt(scalc, ecalc), 1e-9*2.0*NRA*NCA*NCB / dt(scalc, ecalc));

	exit(0);
}
