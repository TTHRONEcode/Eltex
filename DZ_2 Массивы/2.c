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
    array[i] = i + 1; // заполнение массива начинаеся с 1

    printf("%i ", array[i]);
  }

  printf("\nThis array but reversal:\n");

  for (int i = 0; i < N; i++) {
    if (i < N / 2) { // когда заменена половина массива, дальше не менять
      buffer = array[i]; // i элемент записывается в буффер

      array[i] = array[N - i - 1]; // i элемент меняется на (последний - i)
      array[N - i - 1] = buffer; // (последний - i) получает значение с буффера
    }

    printf("%i ", array[i]);
  }

  printf("\n");

  return 0;
}