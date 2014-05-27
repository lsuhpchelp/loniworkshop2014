#include <stdio.h>
#include <time.h>
#include <omp.h>

int main() {
  long long int i, n=500000000;
  float a=2.0;
  float x[n];
  float y[n];
  double start_time, end_time;

  a = 2.0;
#pragma acc data create(x[0:n],y[0:n]) copyin(a)
    {
#pragma acc kernels loop
      for (i = 0; i < n; i++){
	x[i] = 1.0;
	y[i] = 2.0;
      }
      
      start_time = omp_get_wtime();
#pragma acc kernels loop
      {
	for (i = 0; i < n; i++){
	  y[i] = a*x[i] + y[i];
	}
      }
      end_time = omp_get_wtime();
    }
  
  printf ("SAXPY Time: %f\n", end_time - start_time);
  
}
