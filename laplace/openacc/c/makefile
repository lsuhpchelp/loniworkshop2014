CC=pgcc
INC=-I../../serial/
CFLAGS=-acc -Minfo=accel -ta=nvidia,time $(INC)
CFLAGS_S=$(INC)
LAP_ACC=laplace_solver_openacc.c
CUDA_CAST_ACC=laplace2d_acc_ver_cudacast.c

lapacc: $(LAP_ACC)
	$(CC) $(CFLAGS) $(LAP_ACC) -o $@.out

laps: $(LAP_ACC)
	$(CC) $(CFLAGS_S) $(LAP_ACC) -o $@.out

cudaacc: $(CUDA_CAST_ACC)
	$(CC) $(CFLAGS) $(CUDA_CAST_ACC) -o $@.out

cudas: $(CUDA_CAST_ACC)
	$(CC) $(CFLAGS_S) $(CUDA_CAST_ACC) -o $@.out
