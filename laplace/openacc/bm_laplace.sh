#benchmark the openacc version of the jacobi iteration
#!/bin/bash -x
#PBS -q workq
#PBS -A hpc_train_2014
#PBS -l nodes=1:ppn=16
#PBS -l walltime=00:10:00
#PBS -o output_s
#-e eg
#PBS -j oe
#PBS -N acc_laplace

cd $PBS_O_WORKDIR
pgcc -fast laplace_openacc_v0.c
./a.out
pgcc -fast -mp laplace_openacc_v0.c
export OMP_NUM_THREADS=1
./a.out
export OMP_NUM_THREADS=2
./a.out
export OMP_NUM_THREADS=4
./a.out
export OMP_NUM_THREADS=8
./a.out
export OMP_NUM_THREADS=16
./a.out
export PGI_ACC_TIME=1
pgcc -fast -acc -Minfo=accel -ta=nvidia,time laplace_openacc_v0.c
./a.out
pgcc -fast -acc -Minfo=accel -ta=nvidia,time laplace_openacc_v0_dataregion.c
./a.out
