// 1. Вывести квадратную
// матрицу по заданному N.
// Пример N = 3:
// 1 2 3
// 4 5 6
// 7 8 9

#include <stdio.h>

#define N 5

int main() {
  int count = 1;

  int matrix[N][N];

  printf("The size of the square matrix is: %i\nThe matrix itself:\n", N);

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      matrix[i][j] = count;

      count++;

      printf("%i\t", matrix[i][j]);
    }
    printf("\n");
  }

  return 0;
}