all: serialc ompc serialf ompf
serialc:
	rm -rf openmp-mm.o
	gcc -o serialc openmp-mm.c
ompc:
	rm -rf openmp-mm.o
	gcc -fopenmp -o ompc openmp-mm.c
serialf:
	rm -rf openmp-mm.o
	gfortran -o serialf openmp-mm.f90
ompf:
	rm -rf openmp-mm.o
	gfortran -fopenmp -o ompf openmp-mm.f90
clean:
	rm -rf serialc ompc serialf ompf openmp-mm.o

