#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"

int main(int argc, char* argv[])
{

  // Initialize MPI.

  int nprocs, myid;
  MPI_Status status;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  // Initialize the array and find the local max.

  int i, a[10];;
  int max_local=0, max_global;
  for (i=0;i<10;i++) {
    a[i]=random()%1000;
    printf("%d ",a[i]);
    if (max_local <= a[i])
      max_local=a[i];
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if (myid == 0) 
    printf("\n");

  // Send local max to the root process to find the global error.

  if (myid == 0) {
    for(i=1;i<nprocs;i++) {
      MPI_Recv(&max_global,1,MPI_INT,i,0,MPI_COMM_WORLD,&status);
      if (max_global > max_local)
	max_local=max_global;
    }
    max_global=max_local;
  }
  else
    MPI_Send(&max_local,1,MPI_INT,0,0,MPI_COMM_WORLD);

  // Distribute global max to all processes.

  if (myid == 0) {
    for(i=1;i<nprocs;i++)
      MPI_Send(&max_global,1,MPI_INT,i,0,MPI_COMM_WORLD);
  }
  else
    MPI_Recv(&max_global,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);

  printf("Process %d has the global maximum as %d\n",myid,max_global);
  
  // Terminate MPI.

  MPI_Finalize();
  
  return 0;

}
