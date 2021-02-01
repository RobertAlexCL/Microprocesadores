/**
 * ------------
 * TFNilaka.cu
 * ------------
 * AUTH.	Roberto Castillo
 * DATE		2020-11-24
 * ------------
 * Universidad del Valle de Guatemala
 * CC3056 - Programación de Microprocesadores
 * 
 */

#include <stdio.h>
#include <iostream>


#define N 1024
#define bsize 32
__global__ void nilakantha(double* arreglo)
{
	int hilo = blockIdx.x * blockDim.x + threadIdx.x;
	double k = (hilo+1) * 2;
	if (hilo < N){
		if (hilo % 2 == 0){
			arreglo[hilo] = 4/(k*(k+1)*(k+2));
		}
		else{
			
			arreglo[hilo] = -4/(k*(k+1)*(k+2));
		}
		
	}
}

int main()
{
	double *st, *d_st;
	int size = sizeof(double);

	st = (double*)malloc(size*N);
	cudaMalloc((void **) &d_st, size*N);

	nilakantha<<<N/bsize, bsize>>>(d_st);
	
	cudaMemcpy(st, d_st, size*N, cudaMemcpyDeviceToHost);

	double pi = 3;
	int h;
	for(h = 0; h<N; h++;)
		pi += st[h];


	printf("Pi: %.10f\n", pi);

	free(st);
	cudaFree(d_st);
	return 0;
}