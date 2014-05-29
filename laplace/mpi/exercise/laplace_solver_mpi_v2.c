#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"

#define MAXITER 1000000

int main(int argc, char* argv[])
{

  int nprocs, myid;
  int nrl;  

  // Initialize MPI.

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  printf("This is process %d out of %d processes.\n",myid,nprocs);

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

  // Get some timing information.
  double time1=MPI_Wtime();

  // Cap the number of iterations.

  if (niter >= MAXITER) {
    printf("Warnig: Changed the number of iterations to %d.",MAXITER);
    niter=MAXITER;
  }

  if (nr%nprocs != 0) {
    if (myid == 0)
      printf("Error: the number of rows is not multiple of nprocs!\n");
    MPI_Finalize();
    exit(2);
  }

  nrl=nr/nprocs;
  printf("Process %d has row %d through %d\n",myid,myid*nrl+1,(myid+1)*nrl);

  double **t;
  t=malloc((nrl+2)*sizeof(double *));
  for (i=0;i<nrl+2;i++)
    t[i]=malloc((nc+2)*sizeof(double *));

  double **told;
  told=malloc((nrl+2)*sizeof(double *));
  for (i=0;i<nrl+2;i++)
    told[i]=malloc((nc+2)*sizeof(double *));

  // Initialize the array.                                                                                                                                                                           
  for (i=0; i<nrl+2; i++)
    for (j=0; j<nc+2; j++)
      told[i][j]=0.;

  // Set the boundary condition.

  // Right boundary
  double tmin=myid*100.0/nprocs;
  double tmax=(myid+1)*100.0/nprocs;

  for (i=0;i<nrl+2;i++) {
    t[i][nc+1]=tmin+((tmax-tmin)/nrl)*i;
    told[i][nc+1]=t[i][nc+1];
  }

  // Bottom boundary

  if (myid == nprocs-1) {
    for (j=0;j<nc+2;j++) {
      t[nrl+1][j]=(100.0/nc)*j;
      told[nrl+1][j]=t[nrl+1][j];
    }
  }

  // Main loop.

  int tagu=100,tagd=101;
  MPI_Request reqidu,reqidd;
  MPI_Status status;
  double dt,dtg;

  for (iter=1;iter<=niter;iter++) {

    for (i=1;i<=nrl;i++)
      for (j=1;j<=nc;j++)
	t[i][j]=0.25*(told[i+1][j]+told[i-1][j]+told[i][j-1]+told[i][j+1]);

    // Update the boundary data. We'll send above and receive from
    // below first, then send below and receive from above. In both
    // cases, we'll post the receives first, and then do the sends.

    // Send U - Recv D:

    if (myid != nprocs-1)
      MPI_Irecv(t[nrl+1],nc+2,MPI_DOUBLE,myid+1,tagu,MPI_COMM_WORLD,&reqidu);
    if (myid != 0)
      MPI_Send(t[1],nc+2,MPI_DOUBLE,myid-1,tagu,MPI_COMM_WORLD);
    if (myid != nprocs-1)
      MPI_Wait(&reqidu,&status);

    // Send D - Recv U:

    if (myid != 0)
      MPI_Irecv(t[0],nc+2,MPI_DOUBLE,myid-1,tagd,MPI_COMM_WORLD,&reqidd);
    if (myid != nprocs-1)
      MPI_Send(t[nrl],nc+2,MPI_DOUBLE,myid+1,tagd,MPI_COMM_WORLD);
    if (myid != 0)
      MPI_Wait(&reqidd,&status);

    // Check on convergence, and move current values to old
    
    dt=0;
    for (i=1;i<=nrl;i++) {
      for (j=1;j<=nc;j++) {
	dt=fmax(fabs(t[i][j]-told[i][j]),dt);
	told[i][j]=t[i][j];
      }
    }

    // blank 1: Find the global max convergence error with one MPI function call.    



    // Check if output is required.

    if (myid == 0)
      if (iprint != 0)
	if (iter%iprint == 0) 
	  printf("Iteration: %d; Convergence Error: %f\n",iter,dtg);
    
    // Check if convergence criteria meet.
    if (dtg < relerr) {
      printf("\nSolution has converged.\n");
      break;
    }
  }

  free(t);
  free(told);

  // Print out the execution time.
  if (myid == 0) {
    double time2=MPI_Wtime();
    printf("\nTotal Time (sec): %f.\n",time2-time1);
  }

  MPI_Finalize();
  
  return 0;

}
