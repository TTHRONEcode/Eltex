// 2. Вывести заданный массив размером N
// в обратном порядке.
// Пример:
// Ввод > 1 2 3 4 5
// Вывод –> 5 4 3 2 1

#include <stdio.h>

#define N 5

int main() {
  int array[N];

  int buffer;

  printf("This array:\n");

  for (int i = 0; i < N; i++) {
    array[i] = i + 1; // заполнение массива начинаеся с 1, а не с 0

    printf("%i ", array[i]);
  }

  printf("\nThe same array, but inverted:\n");

  for (int i = 0; i < N; i++) {
    if (i < N / 2) { // без этого числа некорректно заменятся
      buffer = array[i];

      array[i] = array[N - i - 1];
      array[N - i - 1] = buffer;
    }

    printf("%i ", array[i]);
  }

  printf("\n");

  return 0;
}