// 2. Вывести заданный массив размером N
// в обратном порядке.
// Пример:
// Ввод > 1 2 3 4 5
// Вывод –> 5 4 3 2 1

#include <stdio.h>

int main() {
  int N, buffer;

  printf("Enter a size of array:\n");

  scanf("%i", &N);

  int array[N];

  printf("This array:\n");

  for (int i = 0; i < N; i++) {
    array[i] = i + 1;

    printf("%i ", array[i]);
  }

  printf("\nThis array but reversal:\n");

  for (int i = 0; i < N; i++) {
    if (i < N / 2) {
      buffer = array[i];

      array[i] = array[N - i - 1];
      array[N - i - 1] = buffer;
    }

    printf("%i ", array[i]);
  }

  printf("\n");

  return 0;
}