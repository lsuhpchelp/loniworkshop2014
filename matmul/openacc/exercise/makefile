# makefile for the matmul
CC=pgcc
FC=pgf90
CFLAGS=-acc -mp -Minfo=all -ta=nvidia,time
MMV0_C=mm_acc_v0.c
MMV1_C=mm_acc_v1.c
MMV0_F=mm_acc_v0.f90
BIN=mmaccv0c mmaccv1c mmaccv0f 

mmaccv0c:
	$(CC) $(CFLAGS) $(MMV0_C) -o $@.out
mmaccv1c:
	$(CC) $(CFLAGS) $(MMV1_C) -o $@.out
mmaccv0f:
	$(FC) $(CFLAGS) $(MMV0_F) -o $@.out
all: $(BIN)
clean:
	rm -rf *.out
