/*
 *  Copyright 2012 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <math.h>
#include <string.h>
#include "timer.h"
#include "dynamic_2d_array.h"

#define NN 4096
#define NM 4096

int main(int argc, char** argv)
{
    const int n = NN;
    const int m = NM;
    const int iter_max = 1000;

    const double tol = 1.0e-6;
    double error     = 1.0;

    real A[n][m];
    real Anew[n][m];

    //real **restrict A   =allocate_dynamic_2d_array(n,m);
    //real **restrict Anew   =allocate_dynamic_2d_array(n,m);

    // Suggest not to do such kind of initialization
    //memset(A[0], 0, n * m * sizeof(double));
    //memset(Anew[0], 0, n * m * sizeof(double));
    for (int i=0; i<n; i++)
        for (int j=0; j<m; j++) 
            A[i][j]=0.0, Anew[i][j]=0.0;

    for (int j = 0; j < n; j++)
    {
        A[j][0]    = 1.0;
        Anew[j][0] = 1.0;
    }

    printf("Jacobi relaxation Calculation: %d x %d mesh\n", n, m);

    StartTimer();
    int iter = 0;

    //#pragma acc data copy(A[0:n][0:m]), create(Anew[0:n][0:m])
#pragma acc data copy(A), create(Anew)
    while ( error > tol && iter < iter_max )
    {
        error = 0.0;

#pragma omp parallel for shared(m, n, Anew, A)
#pragma acc kernels
        for( int j = 1; j < n-1; j++) {
            for( int i = 1; i < m-1; i++ ) {
                Anew[j][i] = 0.25 * ( A[j][i+1] + A[j][i-1]
                        + A[j-1][i] + A[j+1][i]);
                error = fmax( error, fabs(Anew[j][i] - A[j][i]));
            }
        }

#pragma omp parallel for shared(m, n, Anew, A)
#pragma acc kernels
        for( int j = 1; j < n-1; j++) {
            for( int i = 1; i < m-1; i++ ) {
                A[j][i] = Anew[j][i];    
            }
        }

        if(iter % 100 == 0) printf("%5d, %0.6f\n", iter, error);

        iter++;
    }

    //free_dynamic_2d_array(A);
    //free_dynamic_2d_array(Anew);
    real runtime = GetTimer();

    printf(" total: %f s\n", runtime / 1000);
}
