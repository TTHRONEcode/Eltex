// 4. Заполнить матрицу числами от 1 до N^2 улиткой.
// Пример:
// 1  2  3  4  5
// 16 17 18 19 6
// 15 24 25 20 7
// 14 23 22 21 8
// 13 12 11 10 9

#include <stdio.h>

#define N 5

int main() {

  int matrix[N][N];
  int count = 1;
  int lines = 0, columns, offset = 0;

  int digits = 0, divisible = N * N;

  // Главный цикл заполнения
  for (int i = 0; i < N / 2; i++) {

    // горизонтальное заполнение вправо
    for (columns = offset; columns < N - offset; columns++) {
      matrix[lines][columns] = count;
      count++;
    }

    columns--;

    // вертикальное заполнение вниз
    for (lines = offset + 1; lines < N - offset; lines++) {
      matrix[lines][columns] = count;
      count++;
    }

    lines--;

    // горизонтальное заполнение влево
    for (columns = N - offset - 2; columns >= offset; columns--) {
      matrix[lines][columns] = count;
      count++;
    }

    columns++;

    offset++;

    // вертикальное заполнение вверх
    for (lines = N - offset - 1; lines >= offset; lines--) {
      matrix[lines][columns] = count;
      count++;
    }

    lines++;
  }

  // дозаполнение центрального элемента нечётной матрицы
  if (N % 2 == 1)
    matrix[lines][columns + 1] = count;

  // Подсчёт знаков для идеального вывода + выравнивания
  while (divisible > 10) {
    digits++;
    divisible /= 10;
  }

  // Вывод в терминал
  printf("The size of the square matrix is: %i\nThe snail matrix itself:\n", N);

  for (lines = 0; lines < N; lines++) {
    for (columns = 0; columns < N; columns++) {
      printf("%*i", digits + 2, matrix[lines][columns]);
    }
    printf("\n");
  }

  return 0;
}
