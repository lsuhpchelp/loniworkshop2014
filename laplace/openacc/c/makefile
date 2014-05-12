CC=pgcc
INC=-I../../serial/
CFLAGS=-acc -Minfo=accel -ta=nvidia,time $(INC)
#CFLAGS=$(INC)
LAP_ACC=laplace_solver_openacc.c

lapacc: $(LAP_ACC)
	$(CC) $(CFLAGS) $(LAP_ACC) -o $@.out
