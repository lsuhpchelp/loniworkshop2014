# makefile for the matmul
CC=pgcc
CFLAGS=-acc -mp -Minfo=all -ta=nvidia,time
MMV0=mm_acc_v0.c
MMV1=mm_acc_v1.c
BIN=mmaccv0 mmaccv1

mmaccv0:
	$(CC) $(CFLAGS) $(MMV0) -o $@.out
mmaccv1:
	$(CC) $(CFLAGS) $(MMV1) -o $@.out
all: $(BIN)
clean:
	rm -rf *.out
