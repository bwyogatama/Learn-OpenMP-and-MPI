//#include "stdafx.h"
#include <stdio.h>
#include <omp.h>
#include <iostream.h>

#define I 10000
#define J 5000

void matrix_mult(double *result, double *A, double *B, int col, int row) {
	int i, j;
#pragma omp parallel for shared(A,B,result) private(i,j)
	for (i = 0; i<row; i++) {
		double sum = 0;
		double *a = A + i*col;
		result[i] = 0;
#pragma omp parallel for reduction(+:sum)
		for (j = 0; j<col; j++) {
			sum += a[j] * B[j];
		}
		result[i] = sum;
	}
}

int main() {
	double *A, *B, *result;
	double begin, end;
	double time_spent;


	A = (double*)malloc(sizeof(double)* I*J);
	B = (double*)malloc(sizeof(double)* I);
	result = (double*)malloc(sizeof(double)* J);

	begin = omp_get_wtime();
	matrix_mult(result, A, B, I, J);
	end = omp_get_wtime();

	
	printf("%f\n", end - begin);
	system("pause");
	return 0;
}

