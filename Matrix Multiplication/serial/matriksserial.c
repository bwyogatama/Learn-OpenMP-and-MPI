#include <stdio.h>
#include <time.h>

#define row 10
#define col 10
#define N 10
double A[N],B[N], R[N];
int i,j;

/*void matrix_mult(double R, double A, double B,int col, int row){
  int i,j;
  for(i=0; i<row; i++){
    result[i] = 0;
    for(j=0; j<col; j++){
      R[i] += A[j + i*col] * B[j];
    }
  }
}*/

int main(){
  //float *A, *B, *result;
  clock_t begin,end;
  double time_spent;


  //A = (float*)malloc(sizeof(float)* I*J);
 // B = (float*)malloc(sizeof(float)* I); result = (float*)malloc(sizeof(float)* J);

  begin = clock();
  //matrix_mult(R, A, B, I,J);
    for(i=0; i<row; i++){
    R[i] = 0;
    for(j=0; j<col; j++){
      R[i] += A[j + i*col] * B[j];
    }
}
  end = clock();

  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("%f\n", time_spent);
}
