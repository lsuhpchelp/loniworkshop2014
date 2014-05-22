CC=pgcc
INC=-I../../serial/
CFLAGS=-acc -Minfo=accel -ta=nvidia,time $(INC)
CFLAGS_OMP=-mp -Minfo=accel,mp -ta=nvidia,time $(INC)
CFLAGS_S=$(INC)
LAP_ACC=laplace_openacc.c
CUDA_CAST_ACC=laplace_openacc_cudacast.c
MM_ACC=mm_acc.c
PI_ACC=pi_acc.c
SAXPY_ACC=saxpy_acc.c
SAXPY_ACC_FUN=saxpy_acc_fun.c
SAXPY_ACC_1ST=saxpy_1stexample.c

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

piacc: $(PI_ACC)
	$(CC) $(CFLAGS) $(PI_ACC) -o $@.out

pis: $(PI_ACC)
	$(CC) $(CFLAGS_S) $(PI_ACC) -o $@.out

saxpyacc: $(SAXPY_ACC)
	$(CC) $(CFLAGS) $(SAXPY_ACC) -o $@.out

saxpyaccfun: $(SAXPY_ACC_FUN)
	$(CC) $(CFLAGS) $(SAXPY_ACC_FUN) -o $@.out

saxpyacc1st: $(SAXPY_ACC_1ST)
	$(CC) $(CFLAGS) $(SAXPY_ACC_1ST) -o $@.out

