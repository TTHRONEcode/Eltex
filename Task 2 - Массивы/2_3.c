// 3. Заполнить верхний треугольник матрицы 1,
// а нижний 0.
// Пример:
// 0 0 1
// 0 1 1
// 1 1 1

#include <stdio.h>

#define N 5

int main() {
  int count = 0;

  int matrix[N][N];

  printf("The size of the square matrix is: %i\nThe matrix itself:\n", N);

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      matrix[i][j] = ((1 < N - (j + i)) ? 0 : 1);

      printf("%i ", matrix[i][j]);
    }
    printf("\n");
  }

  return 0;
}