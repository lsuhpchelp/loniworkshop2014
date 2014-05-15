CC=pgcc
INC=-I../../serial/
CFLAGS=-acc -mp -Mconcur=allcores -Minfo=accel,mp -ta=nvidia,time $(INC)
CFLAGS_S=$(INC)
LAP_ACC=laplace_openacc.c
CUDA_CAST_ACC=laplace_openacc_cudacast.c
MM_ACC=mm_acc.c

lapacc: $(LAP_ACC)
	$(CC) $(CFLAGS) $(LAP_ACC) -o $@.out

laps: $(LAP_ACC)
	$(CC) $(CFLAGS_S) $(LAP_ACC) -o $@.out

cudaacc: $(CUDA_CAST_ACC)
	$(CC) $(CFLAGS) $(CUDA_CAST_ACC) -o $@.out

cudas: $(CUDA_CAST_ACC)
	$(CC) $(CFLAGS_S) $(CUDA_CAST_ACC) -o $@.out

mmacc: $(MM_ACC)
	$(CC) $(CFLAGS) $(MM_ACC) -o $@.out
