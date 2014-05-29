#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

typedef double real;

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
    int nprows,npcols;

    int nsrow,nscol,iprow,ipcol;
    
    real flops,init_time,start_time,end_time;
    
    MPI_Status status;
    MPI_Datatype vtype;

    real** a;
    real** b;
    real** c;
    real** cfull;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
   
    printf("argc=%d\n",argc); 
    printf("myrank=%d\n",myrank); 
    if (6!=argc) {
        if (0==myrank)
            printf("Usage: ser_mm nrows nprow npcol irow icol\n");
        MPI_Abort(MPI_COMM_WORLD, mpi_err);
        exit(1);
    }
    else {
        nra=atoi(argv[1]);
        nprows=atoi(argv[2]);
        npcols=atoi(argv[3]);
        ipeek=atoi(argv[4]);
        jpeek=atoi(argv[5]);
        printf("%d,%d,%d,%d,%d\n",nra,nprows,npcols,ipeek,jpeek);
    }
    
    nca=nra, ncb=nra;
    
    if (nprocs!=nprows*npcols){
        if (0!=myrank){
            printf("The nubmer of processes is not equal to nprows*npcols!\n");
        }
        MPI_Abort(MPI_COMM_WORLD, mpi_err);
        exit(1);
    }
   
    if (0!=(nra % nprows)) {
        if (0==myrank)
           printf("Nrows is not a multiple of nprocs!\n");
        MPI_Abort(MPI_COMM_WORLD, mpi_err);
        exit(1);
    }
    
    if (0!=(ncb % npcols)) {
        if (0==myrank)
           printf("Nrows is not a multiple of nprocs!\n");
        MPI_Abort(MPI_COMM_WORLD, mpi_err);
        exit(1);
    }
//    
    flops = 2.0*nra*nca*ncb;
    init_time = MPI_Wtime();
    
    iprow= myrank/npcols;
    ipcol= myrank%npcols;
    
    nsrow=nra/nprows;
    nscol=ncb/npcols;

    // allocate memory space for A, B and C
    a=malloc(nsrow*sizeof(real*));
    for (i=0;i<nsrow;i++){
        a[i]=malloc(nca*sizeof(real));
    }
    b=malloc(nca*sizeof(real*));
    for (i=0;i<nca;i++){
        b[i]=malloc(nscol*sizeof(real));
    }
    c=malloc(nsrow*sizeof(real*));
    for (i=0;i<nscol;i++){
        c[i]=malloc(nscol*sizeof(real));
    }
    
    if (0==myrank) {
        cfull=malloc(nra*sizeof(real*));
        for (i=0;i<nra;i++)
            cfull[i]=malloc(ncb*sizeof(real));
    }

    // Initialize sub-A, sub-B and sub-C.
    for (i=0;i<nsrow;i++)
        for (j=0;j<nca;j++)
            a[i][j]=iprow*nsrow+i+j;
    
    for (i=0;i<nca;i++)
        for (j=0;j<nscol;j++)
            b[i][j]=i*(ipcol*nscol+j);
            
    for (i=0;i<nsrow;i++)
        for (j=0;j<nscol;j++)
            c[i][j]=0.0;    
    
    // find out current time
    start_time = MPI_Wtime();
    
    for (i=0;i<nsrow;i++)
        for (j=0;j<nscol;j++)
            for (k=0;k<nca;k++)
                c[i][j] += a[i][k]*b[k][j];

    MPI_Type_vector(nsrow,nscol,ncb,MPI_DOUBLE,&vtype);
    MPI_Type_commit(&vtype);
    
    if (0==myrank){
        for (i=0;i<nsrow;i++)
            for (j=0;j<nscol;j++)
                cfull[i][j]=c[i][j];
        for (i=1;i<=nprocs-1;i++)
            ;//MPI_Recv(cfull(i/npcols*nsrow+1,(i%npcols)*nscol+1),1,vtype,i,1,mpi_comm_world,status);
    }
    else {
        ;//MPI_Send(c,nsrow*nscol,MPI_DOUBLE,0,1,MPI_COMM_WORLD);
    }
    
    MPI_Type_free(&vtype);
    // find out current time
    end_time = MPI_Wtime();
 
   if (0==myrank) {
       printf("Init Time: %7.3f\n", start_time - init_time);
       printf("Calc Time: %7.3f\n", end_time - start_time);
       printf("GFlops: %7.3f\n", 1.0e-9* flops/(end_time - start_time));
   }

   if (0==myrank) {
       printf("Value calculated by the program:\n");
       printf("c[i][j]=%14.6e\n",cfull[ipeek][jpeek]);
       printf("It should be:\n");
       printf("c[i][j]=%14.6e\n",(float)(ipeek)*(float)(jpeek)*(float)(nra)*((float)nra-1.0)/2.0
                               + (float)(jpeek)*(float)(nra)*((float)(nra)-1.0)*(2.0*(float)(nra)-1.0)/6.0);
    }
    
    // free A-C
    for (i=0;i<nsrow;i++){
        free(a[i]);
    }
    free(a);
    for (i=0;i<nca;i++){
        free(b[i]);
    }
    free(b);
    for (i=0;i<nsrow;i++){
        free(c[i]);
    }
    free(c);
    if (0==myrank) {
        for (i=0;i<nra;i++)
            free(cfull[i]);
        free(cfull);
    }

    MPI_Finalize(); 
}
