#define M 1024 
#define N 2048 
#define P 4096 

/* muliply a M lines, P columns matrix by a P lines, N columns matrix
 *    to produce a M lines, N columns matrix */ 
void 
MatrixMultiplication1(restrict float a[M][N], restrict float b[M][P], restrict float c[P][N]) 
{ 
    int i, j, k ; 

#pragma acc region for parallel, vector(8) 
    for (i=0; i<M; i++) { 
#pragma acc for parallel, vector(8) 
        for (j=0; j<N; j++) { 
#pragma acc for seq 
            for (k=0; k<P; k++) 
                a[i][j] += b[i][k]*c[k][j] ; 
        } 
    } 
} 

void 
MatrixMultiplication2(restrict float a[M][N], restrict float b[M][P], restrict float c[P][N]) 
{ 
    int i, j, k ; 

#pragma acc region for parallel, vector(8) 
    for (i=0; i<M; i++){ 
#pragma acc for parallel, vector(8) 
        for (j=0; j<N; j++) { 
            float sum = 0.0 ; 
#pragma acc for seq 
            for (k=0; k<P; k++) 
                sum += b[i][k]*c[k][j] ; 
            a[i][j] = sum ; 
        } 
    } 
} 

void 
MatrixMultiplication3(float * restrict a, float * restrict b, float * restrict c, int m, int n, int p) 
{ 
    int i, j, k ; 

#pragma acc data region copyout(a[0:(m*n)-1]), copyin(b[0:(m*p)-1],c[0:(p*n)-1]) 
    { 
#pragma acc region for parallel, vector(8) 
        for (i=0; i<m; i++){ 
#pragma acc for parallel, vector (8) 
            for (j=0; j<n; j++) { 
#pragma acc for seq 
                for (k=0; k<p; k++) 
                    a[i*n+j] += b[i*p+k]*c[k*n+j] ; 
            } 
        } 
    } 
} 

void 
MatrixMultiplication4(float * restrict a,float * restrict b, float * restrict c, int m, int n, int p) 
{ 
    int i, j, k ; 

#pragma acc data region copyout(a[0:(m*n)-1]), copyin(b[0:(m*p)-1],c[0:(p*n)-1]) 
    { 
#pragma acc region for parallel, vector(8) 
        for (i=0; i<m; i++){ 
#pragma acc for parallel, vector (8) 
            for (j=0; j<n; j++) 
            { 
                float sum = 0.0 ; 
#pragma acc for seq 
                for (k=0; k<p; k++) 
                    sum += b[i*p+k]*c[k*n+j] ; 
                a[i*n+j] = sum ; 
            } 
        } 
    } 
} 
