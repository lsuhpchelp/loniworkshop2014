# makefile for the pi calculation
CC=pgcc
FC=pgf90
CFLAGS=-acc -mp -Minfo=all -ta=nvidia,time
SAXPY_C=saxpy_acc.c
SAXPY_F=saxpy_acc.f90
BIN=saxpyacc_c saxpyacc_f

saxpyacc_c:
	$(CC) $(CFLAGS) $(SAXPY_C) -o $@.out
saxpyacc_f:
	$(FC) $(CFLAGS) $(SAXPY_F) -o $@.out
all: $(BIN)
clean:
	rm -rf *.out
