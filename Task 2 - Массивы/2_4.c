// 4. Заполнить матрицу числами от 1 до N^2 улиткой.
// Пример:
// 1  2  3  4  5
// 16 17 18 19 6
// 15 24 25 20 7
// 14 23 22 21 8
// 13 12 11 10 9

#include <stdio.h>

#define N 6

int main() {

  int matrix[N][N] = {0};

  int count = 1;
  int lines = 0, columns = 0, spins = 0;
  int divisible, digits = 0;

  printf("The size of the square matrix is: %i\nThe snail matrix itself:\n", N);

  ///////////////////////

  for (; columns < N - spins; columns++) {
    matrix[lines][columns] = count;
    count++;
  }

  columns--;
  lines++;

  ///////////////////////

  while (spins < N / 2) {

    for (; lines < N - spins; lines++) {
      matrix[lines][columns] = count;
      count++;
    }

    lines--;
    columns--;

    for (; columns >= spins; columns--) {
      matrix[lines][columns] = count;
      count++;
    }

    columns++;
    lines--;

    spins++;

    for (; lines >= spins; lines--) {
      matrix[lines][columns] = count;
      count++;
    }

    lines++;
    columns++;

    for (; columns < N - spins; columns++) {
      matrix[lines][columns] = count;
      count++;
    }

    columns--;
    lines++;
  }

  ///////////////////////

  divisible = N * N;

  do {
    if (divisible > 10)
      digits++;
    divisible /= 10;

  } while (divisible > 10);

  ///////////////////////

  for (lines = 0; lines < N; lines++) {
    for (columns = 0; columns < N; columns++) {
      printf("%-*i", digits + 2, matrix[lines][columns]);
    }
    printf("\n");
  }

  return 0;
}