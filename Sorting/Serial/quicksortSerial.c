#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10000
#define X 1000

void quicksort(int *, int, int);
int partition(int *, int, int);
int choosePivot(int *, int, int);
void swap(int *, int *);

int main(int argc, char ** argv)
{
  //Timer start
	clock_t begin = clock();
  srand(123456);

  int * arr = (int *) malloc(sizeof(int)*N);
  int i;
  for(i = 0; i < N; i++)
    arr[i] = rand()%X;

  quicksort(arr, 0, N-1);
  //Timer Stop
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  for(i = 0; i < N; i++)
    printf("%d ", arr[i]);

  free((void *) arr);
  printf("\n");
  printf("%f",time_spent);
  return 0;
}

void quicksort(int * arr, int lo, int hi)
{
  if(lo < hi)
  {
    int p = partition(arr, lo, hi);
    quicksort(arr, lo, p - 1);
    quicksort(arr, p + 1, hi);
  }
}

int partition(int * arr, int lo, int hi)
{
  int i;
  int pivotIdx = choosePivot(arr, lo, hi);
  int pivotVal = arr[pivotIdx];

  swap(&arr[pivotIdx], &arr[hi]);

  int storeIdx = lo;

  for(i = lo; i < hi; i++)
  {
    if(arr[i] < pivotVal)
    {
      swap(&arr[i], &arr[storeIdx]);
      storeIdx++;
    }
  }

  swap(&arr[storeIdx], &arr[hi]);
  return storeIdx;
}

void swap(int * x, int * y)
{
  int temp = *x;
  *x = *y;
  *y = temp;
}

//Select the median of arr[lo], arr[hi], and arr[(lo+hi)/2]
int choosePivot(int * arr, int lo, int hi)
{
  int mid = (lo+hi)/2;

  int temp;
  if(arr[lo] > arr[hi])
  {
    temp = lo;
    lo = hi;
    hi = temp;
  }
  if(arr[mid] < arr[lo])
  {
    temp = mid;
    mid = lo;
    lo = temp;
  }
  if(arr[hi] < arr[mid])
  {
    temp = mid;
    mid = hi;
    hi = temp;
  }
  return mid;
}
