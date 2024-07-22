#include "8_1_calc_func.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

enum { ADD, MUL, DIV, EXIT, SUB };

struct CalcFuncsBook {
  int num;
  char name[256];
  char expression[256];
  long (*func)(long, long);
};

struct CalcFuncsBook CalcFuncs[5] = {
    {ADD, "Сложение", "a + b", Calc_Proc_Add}, /////
    {SUB, "Вычитание", "a - b", Calc_Proc_Sub},
    {MUL, "Умножение", "a * b", Calc_Proc_Mul},
    {DIV, "Деление", "a / b", Calc_Proc_Div},
    {EXIT, "Выход", "Выходим...", NULL}};
int g_max_num = sizeof(CalcFuncs) / sizeof(CalcFuncs[0]);

void CalcExit() { exit(0); };

void Util_ClearScanf() {
  int c;
  while ((c = fgetc(stdin)) != EOF && c != '\n')
    ;
}

long CalcStdIn(char letter) {

  long number = 0;

  do {
    printf("*Введите число %c:\n*%c = ", letter, letter);

    scanf("%20ld", &number);
    Util_ClearScanf();

    if (number >= INT_MAX) {
      printf("*Это число за гранью дозволенного! (%c >= %d)\n"
             "*Нужно ввести число поменьше\n\n",
             letter, INT_MAX);
    } else if (number <= INT_MIN) {
      printf("*Это число слишком микроскопическое! (%c <= %d)\n"
             "*Нужно ввести число побольше\n\n",
             letter, INT_MIN);
    }

  } while (number >= INT_MAX || number <= INT_MIN);

  return number;
}

void CalcStdOut(int calc_num) {
  long number_a, number_b;

  printf("*%d) ", calc_num);
  calc_num--;
  for (int i = 0; i < g_max_num; i++) {
    if (CalcFuncs[i].num == calc_num) {

      if (calc_num != EXIT) {

        printf("%s c = %s\n", CalcFuncs[i].name, CalcFuncs[i].expression);

        number_a = CalcStdIn('a');
        number_b = CalcStdIn('b');

        printf("\n*Результат:\n*c = ");

        printf("%ld\n", CalcFuncs[i].func(number_a, number_b));
      } else {
        printf("%s\n*%s\n", CalcFuncs[i].name, CalcFuncs[i].expression);
        CalcExit();
      }
    }
  }
}

void CalcMenu() {
  int menu_num;

  while (1) {
    printf("\n*Калькулятор\n\n");
    for (int i = 0; i < g_max_num; i++) {
      for (int j = 0; j < g_max_num; j++) {
        if (CalcFuncs[j].num == i) {
          printf("%d) %s\n", i + 1, CalcFuncs[j].name);
          break;
        }
      }
    }

    printf("\n*Выберите пункт меню: ");
    scanf("%9d", &menu_num);
    Util_ClearScanf();

    while (menu_num < 1 ||
           menu_num > (sizeof(CalcFuncs) / sizeof(CalcFuncs[0]))) {
      printf("\n*Нужно ввести число от "
             "1 до %d!\n",
             EXIT);
      printf("*Выберите пункт меню: ");
      scanf("%1d", &menu_num);
      Util_ClearScanf();
    }
    printf("\n");

    CalcStdOut(menu_num);
  }
}

int main() {

  CalcMenu();

  return 0;
}
