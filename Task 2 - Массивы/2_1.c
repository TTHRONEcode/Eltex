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

  printf("The size of the square matrix is: %i\n", N);

  printf("This matrix itself:\n");

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
// 3. Заполнить верхний треугольник матрицы 1,
// а нижний 0.
// Пример:
// 0 0 1
// 0 1 1
// 1 1 1
// 4. Заполнить матрицу числами от 1 до N
// 2
// улиткой.
// Пример:
// 1 2 3 4 5
// 16 17 18 19 6
// 15 24 25 20 7
// 14 23 22 21 8
// 13 12 11 10