#include <stdio.h>

// 3. Найти количество единиц в двоичном представлении целого положительного
// числа(число вводится с клавиатуры).

int main() {
  int a;

  int b = 0;

  printf("Enter a positive integer: ");

  scanf("%i", &a);

  for (int i = 0; i < 32; i++) {
    b += a & 1;
    a = a >> 1;
  }

  printf("It's number of '1': %i\n", b);

  return 0;
}