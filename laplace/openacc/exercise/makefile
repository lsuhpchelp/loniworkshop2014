CC=pgcc
FC=pgf90
#CFLAGS=-acc -Minfo=accel -ta=nvidia,time $(INC)
CFLAGS_ACC=-acc -fast -Minfo=all -ta=nvidia,time
CFLAGS_OMP=-mp -fast -Minfo=all
CFLAGS_SER=-fast -Minfo=all 
LAP_ACC_V0=laplace_openacc_v0.c
LAP_ACC_V0_F=laplace_openacc_v0.f90
LAP_ACC_V1=laplace_openacc_v1.c
BIN=lap_acc_v0 lap_acc_v0f lap_acc_v0_dr lap_acc_v1 lap_omp lap_serial

lap_acc_v0: $(LAP_ACC)
	$(CC) $(CFLAGS_ACC) $(LAP_ACC_V0) -o $@.out

lap_acc_v0f: $(LAP_ACC)
	$(FC) $(CFLAGS_ACC) $(LAP_ACC_V0_F) -o $@.out

lap_acc_v1: $(LAP_ACC)
	$(CC) $(CFLAGS_ACC) $(LAP_ACC_V1) -o $@.out

lap_omp: $(LAP_ACC)
	$(CC) $(CFLAGS_OMP) $(LAP_ACC_V0) -o $@.out

lap_serial: $(LAP_ACC)
	$(CC) $(CFLAGS_SER) $(LAP_ACC_V0) -o $@.out

all: $(BIN)
clean:
	rm -rf *.out
