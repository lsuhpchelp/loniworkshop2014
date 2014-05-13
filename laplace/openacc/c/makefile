CC=pgcc
INC=-I../../serial/
CFLAGS=-acc -Minfo=accel -ta=nvidia,time $(INC)
#CFLAGS=$(INC)
LAP_ACC=laplace_solver_openacc.c
CUDA_CAST_ACC=laplace2d_acc_ver_cudacast.c

lapacc: $(LAP_ACC)
	$(CC) $(CFLAGS) $(LAP_ACC) -o $@.out

cudacast: $(CUDA_CAST_ACC)
	$(CC) $(CFLAGS) $(CUDA_CAST_ACC) -o $@.out
