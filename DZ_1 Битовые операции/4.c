#include <stdio.h>

// 4. Поменять в целом положительном числе (типа int) значение третьего
// байта на введенное пользователем число (изначальное число также
// вводится с клавиатуры)

int main() {

  int a, b;

  printf("Enter a positive integer: \n");

  scanf("%i", &a);

  printf("Enter a positive integer fot third byte (0-255): \n");

  scanf("%i", &b);

  a = (a & 0xFF00FFFF) | (b << 16);

  printf("Final integer with changed third byte:\n%i\n", a);

  return 0;
}