// Задание на функции
// 2. Имеется программа (исходный код которой приводится ниже,
// компилировать с ключами: -fno-stack-protector -no-pie).
//
// Вам необходимо
// произвести анализ программы с помощью отладчика для выяснения длины
// массива для ввода пароля и адреса ветки условия проверки корректности
// ввода пароля, которая выполняется при условии совпадения паролей.
//
// Ввести пароль (строку символов)таким образом, чтобы перезаписать адрес
// возврата на выясненный адрес (есть символы которые нельзя ввести с
// клавиатуры, поэтому можно использовать перенаправление ввода(<) при
// запуске программы).

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// int IsPassOk(void);
// int main(void)
// {
// int PwStatus;
// puts("Enter password:");
// PwStatus = IsPassOk();
// if (PwStatus == 0) {
// printf("Bad password!\n");
// exit(1);
// } else {
// printf("Access granted!\n"); // Строка для которой нужно
//  выяснить адрес
// }
// return 0;
// }
// int IsPassOk(void)
// {
// char Pass[12];
// gets(Pass);
// return 0 == strcmp(Pass, "test");
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int IsPassOk(void);
int main(void) {
  int PwStatus;
  puts("Enter password:");
  PwStatus = IsPassOk();
  if (PwStatus == 0) {
    printf("Bad password!\n");
    exit(1);
  } else {
    printf("Access granted!\n"); // Строка для которой нужно выяснить адрес
  }
  return 0;
}
int IsPassOk(void) {
  char Pass[12];
  gets(Pass);
  return 0 == strcmp(Pass, "test");
}
