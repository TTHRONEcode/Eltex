// 1. Вывести двоичное представление целого положительного числа,
// используя битовые операции(число вводится с клавиатуры).
// 2. Вывести двоичное представление целого отрицательного числа,
// используя битовые операции(число вводится с клавиатуры).

#include <stdio.h>

int main() {

  int a;

  printf("Enter a positive or negative integer:\n");

  scanf("%i", &a);

  printf("It's binary form:\n");

  for (int i = ((sizeof(a) * 8) - 1); i >= 0; i--) {
    printf("%i", (a >> i) & 1);

    if (i % 8 == 0) // это нужно чтобы были пробелы между 8 битами
      printf(" ");

    // Обратный отсчёт i нужен, чтобы двоичная форма числа была в привычном
    // виде (справа на лево)
  }

  printf("\n");

  return 0;
}