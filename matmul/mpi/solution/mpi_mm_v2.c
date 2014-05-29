#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

typedef double real;
#define N 100

int main (int argc, char *argv[])
{
    int nra,
        nca,
        ncb,
        nprocs,
        myrank,
        mpi_err=99,
        errcode;
    
    int i,j,k;
    int ipeek,jpeek;
    
    int rows,ira_start,ira_end;
    
    real flops,init_time,start_time,end_time;
    
    MPI_Status status;

    // real** a;
    // real** b;
    // real** c;
    
    real a[N][N];
    real b[N][N];
    real c[N][N];
    

    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
   
    if (4!=argc) {
        if (0==myrank)
            printf("Usage: ser_mm nrows irow icol\n");
        MPI_Abort(MPI_COMM_WORLD, mpi_err);
        exit(1);
    }
    else {
        nra=atoi(argv[1]);
        ipeek=atoi(argv[2]);
        jpeek=atoi(argv[3]);
    }
    
    nra=N;
    
    nca=nra, ncb=nra;
    if (0!=(nra % nprocs)) {
        if (0==myrank)
           printf("Nrows is not a multiple of nprocs!\n");
        MPI_Abort(MPI_COMM_WORLD, mpi_err);
        exit(1);
    }
//    
    flops = 2.0*nra*nca*ncb;
    init_time = MPI_Wtime();

    // allocate memory space for A, B and C
    // a=malloc(nra*sizeof(real*));
    // for (i=0;i<nra;i++){
        // a[i]=malloc(nca*sizeof(real));
    // }
    // b=malloc(nca*sizeof(real*));
    // for (i=0;i<nca;i++){
        // b[i]=malloc(ncb*sizeof(real));
    // }
    // c=malloc(nra*sizeof(real*));
    // for (i=0;i<nra;i++){
        // c[i]=malloc(ncb*sizeof(real));
    // }
    
    // initialize the values
    for (i=0;i<nra;i++)
        for (j=0;j<nca;j++)
            a[i][j]=i+j;
    
    for (i=0;i<nca;i++)
        for (j=0;j<ncb;j++)
            b[i][j]=i*j;
            
    for (i=0;i<nra;i++)
        for (j=0;j<ncb;j++)
            c[i][j]=0.0;    
    
    // find out current time
    start_time = MPI_Wtime();

    rows=nra/nprocs;
    ira_start=myrank*rows;
    ira_end=(myrank+1)*rows-1;
    
    //printf("ira_start=%d,ira_end=%d\n",ira_start,ira_end);
    
    for (i=ira_start;i<=ira_end;i++)
        for (j=0;j<ncb;j++)
	{
            for (k=0;k<nca;k++)
                c[i][j] += a[i][k]*b[k][j];
	}
    
    if (0==myrank) {
        for (i=1;i<=nprocs-1;i++) {
	    MPI_Recv(c[i*rows],rows*nca,MPI_DOUBLE,i,1,MPI_COMM_WORLD,&status);
        }
    }
    else {
        MPI_Send(c[myrank*rows],rows*nca,MPI_DOUBLE,0,1,MPI_COMM_WORLD);
    }
    
    // find out current time
    end_time = MPI_Wtime();
 
   if (0==myrank) {
       printf("Init Time: %7.3e\n", start_time - init_time);
       printf("Calc Time: %7.3e\n", end_time - start_time);
       printf("GFlops: %7.3e\n", 1.0e-9* flops/(end_time - start_time));
   }

   //if (ipeek%nprocs==myrank) {
   if (0==myrank) {
       printf("Value calculated by the program:\n");
       printf("c[%d][%d]=%14.6e\n",ipeek,jpeek,c[ipeek][jpeek]);
       printf("It should be:\n");
       printf("c[%d][%d]=%14.6e\n",ipeek,jpeek,(float)(ipeek)*(float)(jpeek)*(float)(nra)*((float)nra-1.0)/2.0
                               + (float)(jpeek)*(float)(nra)*((float)(nra)-1.0)*(2.0*(float)(nra)-1.0)/6.0);
    }
    
    // free A-C
    // for (i=0;i<nra;i++){
        // free(a[i]);
    // }
    // free(a);
    // for (i=0;i<nca;i++){
        // free(b[i]);
    // }
    // free(b);
    // for (i=0;i<nra;i++){
        // free(c[i]);
    // }
    // free(c);

    MPI_Finalize(); 
}
