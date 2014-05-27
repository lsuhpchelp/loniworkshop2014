#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"

#define MAXITER 1000000

int main(int argc, char* argv[])
{

  int nprocs, myid;
  int nrl,ncl; 
  int myrowid,mycolid;

  // Initialize MPI.

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  printf("This is process %d out of %d processes.\n",myid,nprocs);

  // Exit if the number of arguments is not 5.
  if (argc != 7) {
    printf("Usage: laplace nrows ncols npcols niter iprint relerr\n");
    exit(1);
  }

  // Process the command line arguments.

  int nr=atoi(argv[1]);
  int nc=atoi(argv[2]);
  int nprocr,nprocc=atoi(argv[3]);
  int niter=atoi(argv[4]);
  int iprint=atoi(argv[5]);
  double relerr=atof(argv[6]);

  int i,j,iter;

  // Get some timing information.
  double time1=MPI_Wtime();

  // Cap the number of iterations.

  if (niter >= MAXITER) {
    printf("Warnig: Changed the number of iterations to %d.",MAXITER);
    niter=MAXITER;
  }

  // Check the number of columns in the process grid.

  if (nprocs%nprocc != 0) {
    if (myid == 0) 
      printf("Error: the number of processes are not multiples of npcols!\n");
    MPI_Finalize();
    exit(2);
  }

  nprocr=nprocs/nprocc;
  myrowid=myid/nprocc;
  mycolid=myid-nprocc*myrowid;

  // Check the rows and columns.

  if (nr%nprocr != 0) {
    if (myid == 0)
      printf("Error: the number of rows is not multiple of the number of rows in the process grid!\n");
    MPI_Finalize();
    exit(3);
  }

  if (nc%nprocc != 0) {
    if (myid == 0)
      printf("Error: the number of columns is not multiple of the number of columns in the process grid!\n");
    MPI_Finalize();
    exit(4);
  }

  nrl=nr/nprocr;
  ncl=nc/nprocc;

  double **t;
  t=malloc((nrl+2)*sizeof(double *));
  for (i=0;i<nrl+2;i++)
    t[i]=malloc((ncl+2)*sizeof(double *));

  double **told;
  told=malloc((nrl+2)*sizeof(double *));
  for (i=0;i<nrl+2;i++)
    told[i]=malloc((ncl+2)*sizeof(double *));

  // Initialize the array.

  for (i=0; i<nrl+2; i++)
    for (j=0; j<ncl+2; j++)
      told[i][j]=0.;

  // Set the boundary condition.

  // Right boundary

  double tmin=myrowid*100.0/nprocr;
  double tmax=(myrowid+1)*100.0/nprocr;

  if (mycolid == nprocc-1) {
    for (i=0;i<nrl+2;i++) {
      t[i][ncl+1]=tmin+((tmax-tmin)/nrl)*i;
      told[i][ncl+1]=t[i][ncl+1];
    }
  }

  // Bottom boundary

  tmin=mycolid*100.0/nprocc;
  tmax=(mycolid+1)*100.0/nprocc;

  if (myrowid == nprocr-1) {
    for (j=0;j<ncl+2;j++) {
      t[nrl+1][j]=tmin+((tmax-tmin)/ncl)*j;
      told[nrl+1][j]=t[nrl+1][j];
    }
  }

  // Main loop.

  int tagu=100,tagd=101,tagr=102,tagl=103;
  MPI_Request reqid_rl,reqid_rr,reqid_ru,reqid_rd;
  MPI_Request reqid_sl,reqid_sr,reqid_su,reqid_sd;
  MPI_Status status;
  double dt,dtg;

  // Define buffers to pass the column boundary data.

  double *srbuffer=malloc((nrl+2)*sizeof(double *));  
  double *slbuffer=malloc((nrl+2)*sizeof(double *));
  double *rrbuffer=malloc((nrl+2)*sizeof(double *));  
  double *rlbuffer=malloc((nrl+2)*sizeof(double *));  

  // Main loop starts here.

  for (iter=1;iter<=niter;iter++) {

    for (i=1;i<=nrl;i++)
      for (j=1;j<=ncl;j++)
	t[i][j]=0.25*(told[i+1][j]+told[i-1][j]+told[i][j-1]+told[i][j+1]);

    // Update the boundary data. We'll send above and receive from
    // below first, then send below and receive from above. In both
    // cases, we'll post the receives first, and then do the sends.

    // Fill the send buffer.

    if (mycolid != 0) 
      for (i=1;i<=nrl;i++)
	slbuffer[i]=t[i][1];

    if (mycolid != nprocc-1)
      for (i=1;i<=nrl;i++)
	srbuffer[i]=t[i][ncl];

    // Send U - Recv D:

    if (myrowid != nprocr-1)
      MPI_Irecv(t[nrl+1],ncl+2,MPI_DOUBLE,myid+nprocc,tagu,MPI_COMM_WORLD,&reqid_ru);
    if (myrowid != 0)
      MPI_Isend(t[1],ncl+2,MPI_DOUBLE,myid-nprocc,tagu,MPI_COMM_WORLD,&reqid_su);

    // Send D - Recv U:

    if (myrowid != 0)
      MPI_Irecv(t[0],ncl+2,MPI_DOUBLE,myid-nprocc,tagd,MPI_COMM_WORLD,&reqid_rd);
    if (myrowid != nprocr-1)
      MPI_Isend(t[nrl],ncl+2,MPI_DOUBLE,myid+nprocc,tagd,MPI_COMM_WORLD,&reqid_sd);

    // Send R - Recv L:

    if (mycolid != 0)
      MPI_Irecv(rlbuffer,nrl+2,MPI_DOUBLE,myid-1,tagr,MPI_COMM_WORLD,&reqid_rr);
    if (mycolid != nprocc-1)
      MPI_Isend(srbuffer,nrl+2,MPI_DOUBLE,myid+1,tagr,MPI_COMM_WORLD,&reqid_sr);

    // Send L - Recv R:

    if (mycolid != nprocc-1)
      MPI_Irecv(rrbuffer,nrl+2,MPI_DOUBLE,myid+1,tagl,MPI_COMM_WORLD,&reqid_rl);
    if (mycolid != 0)
      MPI_Isend(slbuffer,nrl+2,MPI_DOUBLE,myid-1,tagl,MPI_COMM_WORLD,&reqid_sl);

    // Wait for the left and right sends and receives.

    if (mycolid != 0) {
      MPI_Wait(&reqid_sl,&status);
      MPI_Wait(&reqid_rr,&status);
    }
    if (mycolid != nprocc-1) {
      MPI_Wait(&reqid_sr,&status);
      MPI_Wait(&reqid_rl,&status);
    }

    // Unpack the left and right column data.
    
    if (mycolid != 0)
      for (i=1;i<=nrl;i++)
	t[i][0]=rlbuffer[i];

    if (mycolid != nprocc-1)
      for (i=1;i<=nrl;i++)
	t[i][ncl+1]=rrbuffer[i];

    // Wait for the above and below sends and receives.

    if (myrowid != 0) {
      MPI_Wait(&reqid_su,&status);
      MPI_Wait(&reqid_rd,&status);
    }
    if (myrowid != nprocr-1) {
      MPI_Wait(&reqid_sd,&status);
      MPI_Wait(&reqid_ru,&status);
    }

    // Check for sL rR

    /*    if (myid == 3) {
      for (i=0;i<nrl+2;i++)
	printf("%f ",t[i][1]);
      printf("\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (myid == 2) {
      for (i=0;i<nrl+2;i++)
	printf("%f ",t[i][ncl+1]);
      printf("\n");
      } */

    // Check on convergence
    
    dt=0;
    for (i=1;i<=nrl;i++) {
      for (j=1;j<=ncl;j++) {
	dt=fmax(fabs(t[i][j]-told[i][j]),dt);
	told[i][j]=t[i][j];
      }
    }

    // Find the global max convergence error.    

    MPI_Allreduce(&dt,&dtg,1,MPI_DOUBLE,MPI_MAX,MPI_COMM_WORLD);

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
