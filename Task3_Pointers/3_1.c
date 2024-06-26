// 1. Поменять в целом положительном числе (типа int) значение третьего
// байта на введенное пользователем число (изначальное число также
// вводится с клавиатуры) через указатель (не применяя битовые
// операции).

#include <stdio.h>

int main() {
  int a, b;
  char *ptr = (char *)&a;

  printf("Enter a positive integer: \n");
  scanf("%i", &a);

  printf("Enter a positive integer fot third byte (0-255): \n");
  scanf("%i", &b);

  ptr = ptr + 1;
  *ptr = b;

  printf("Final integer with changed third byte:\n%i\n", a);

  return 0;
}