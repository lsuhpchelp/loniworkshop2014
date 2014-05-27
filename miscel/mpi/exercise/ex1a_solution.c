#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char* argv[])
{

  // Initialize MPI.

  MPI_Init(&argc,&argv);
  int nprocs, myid;
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  // Print out to screen.

  if (myid%2 == 0) 
    printf("Process %d has the color red.\n",myid);
  else
    printf("Process %d has the color green.\n",myid);

  // Finalize MPI.

  MPI_Finalize();

  return 0;

}
