#!/bin/bash -x
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
