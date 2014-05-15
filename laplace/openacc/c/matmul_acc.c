#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define dt(start, end)  ((end.tv_sec - start.tv_sec) + \
			 1/1000000.0*(end.tv_usec - start.tv_usec))

int main() {
  int i,j,k;
  int nra=1500, nca=2000, ncb=1000;
  double a[nra][nca],b[nca][ncb],c[nra][ncb];
  struct timeval icalc, scalc, ecalc;
  double flops, sum, timing ;
  
  flops = 2.0 * nra * nca * ncb;

#pragma acc data create(a,b,c)
  {
    gettimeofday(&icalc, NULL);
    for (i = 0; i < nra; i++){
      for (j = 0; j < nca; j++){
	a[i][j] = (double)(i+j);
      }
    }
    for (j = 0; j < nca; j++){
      for (k = 0; k < ncb; k++){
	b[j][k] = (double)(i*j);
      }
    }
    for (i = 0; i < nra; i++){
      for (k = 0; k < ncb; k++){
	c[i][k] = 0.0;
      }
    }
    
    gettimeofday(&scalc, NULL);
#pragma acc parallel loop private(sum)
    for (i = 0; i < nra; i++){
      for (k = 0; k < ncb; k++){
	sum = 0.0;
#pragma acc loop seq
	for (j = 0; j < nca; j++){
	  sum = sum + a[i][j] * b[j][k];
	}
	c[i][k] = sum;
      }
    }
    gettimeofday(&ecalc, NULL);
  }
  timing = dt(scalc,ecalc);
  printf("Init Time: %6.3f Calc Time: %6.3f GFlops: %7.3f\n",dt(icalc,scalc),timing,1e-9*flops/timing );
}