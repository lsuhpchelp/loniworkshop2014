# makefile for the pi calculation
CC=pgcc
FC=pgf90
CFLAGS=-acc -mp -Minfo=all -ta=nvidia,time
PI_C=pi_acc.c
PI_F=pi_acc.f90
BIN=piacc_c piacc_f

piacc_c:
	$(CC) $(CFLAGS) $(PI_C) -o $@.out
piacc_f:
	$(FC) $(CFLAGS) $(PI_F) -o $@.out
all: $(BIN)
clean:
	rm -rf *.out
