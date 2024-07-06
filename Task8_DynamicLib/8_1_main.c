#include "8_1_calc_func.h"//
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

enum { ADD = 1, SUB, MUL, DIV, EXIT };

void Std_ClearScanf() {
  int c;
  while ((c = fgetc(stdin)) != EOF && c != '\n')
    ;
}

long Calc_Std_In(char letter) {

  long number = 0;

  do {
    printf("*Введите число %c:\n*%c = ", letter, letter);

    scanf("%20ld", &number);
    Std_ClearScanf();

    if (number >= INT_MAX) {
      printf("*Введено слишком большое число! (>= %d)\n"
             "*Нужно ввести число поменьше\n\n",
             INT_MAX);
    } else if (number <= INT_MIN) {
      printf("*Введено слишком маленькое число! (<= %d)\n"
             "*Нужно ввести число побольше\n\n",
             INT_MIN);
    }

  } while (number >= INT_MAX || number <= INT_MIN);

  return number;
}

void Calc_Std_Out(int calc_num) {
  long number_a = 0, number_b = 0;

  printf("*%d) ", calc_num);

  for (int i = 0; i < 2; i++) {
    switch (calc_num) {
    case ADD:
      if (i == 0)
        printf("Сложение c = (a + b)\n");
      else
        printf("%ld\n", Calc_Proc_Add(number_a, number_b));
      break;

    case SUB:
      if (i == 0)
        printf("Вычитание c = (a - b)\n");
      else
        printf("%ld\n", Calc_Proc_Sub(number_a, number_b));
      break;

    case MUL:
      if (i == 0)
        printf("Умножение c = (a * b)\n");
      else
        printf("%ld\n", Calc_Proc_Mul(number_a, number_b));
      break;

    case DIV:
      if (i == 0)
        printf("Деление c = (a / b)\n");
      else
        printf("%ld\n", Calc_Proc_Div(number_a, number_b));
      break;

    case EXIT:
      printf("Выход\n*Выходим...\n");
      exit(0);
      break;
    }

    if (i == 0) {
      number_a = Calc_Std_In('a');
      number_b = Calc_Std_In('b');

      printf("\n*Результат:\n*c = ");
    }
  }
}

void Calc_Menu() {
  int menu_num;

  while (menu_num != EXIT) {
    printf("\n*Калькулятор*\n\n"
           "%d) Сложение\n"
           "%d) Вычитание\n"
           "%d) Умножение\n"
           "%d) Деление\n"
           "%d) Выход\n",
           ADD, SUB, MUL, DIV, EXIT);
    printf("*Выберите пункт меню: ");
    scanf("%1d", &menu_num);
    Std_ClearScanf();

    while (menu_num < ADD || menu_num > EXIT) {
      printf("\n*Нужно ввести число от 1 до 5!\n");
      printf("*Выберите пункт меню: ");
      scanf("%1d", &menu_num);
        Std_ClearScanf();
    }
    printf("\n");

    Calc_Std_Out(menu_num);
  }
}

int main() {

  Calc_Menu();

  return 0;
}
