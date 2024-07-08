#include "8_1_1_calc_func.h"
#include <dlfcn.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static void *handle = NULL;
static char *func_name;
static long (*func_ptr)(long, long) = NULL;

struct CalcFuncsBook {
  char name[128];
  char expression[128];
  long (*func)(long, long);
};

struct CalcFuncsBook CalcFuncs[2] = {{"Выход", "Выходим...", NULL},
                                     {"_", "_", NULL}};
int g_max_num;

void CalcExit() { exit(0); };

static void Util_ClearScanf() {
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

  if (calc_num != 0) {

    printf("%s c = %s\n", CalcFuncs[calc_num].name,
           CalcFuncs[calc_num].expression);

    number_a = CalcStdIn('a');
    number_b = CalcStdIn('b');

    printf("\n*Результат:\n*c = ");

    printf("%ld\n", CalcFuncs[calc_num].func(number_a, number_b));
  } else {
    printf("%s\n*%s\n", CalcFuncs[calc_num].name,
           CalcFuncs[calc_num].expression);
    CalcExit();
  }
}

void CalcMenu() {
  int menu_num;

  while (1) {
    printf("\n*Калькулятор\n\n");
    for (int i = 0; i < g_max_num; i++) {
      printf("%d) %s\n", i + 1, CalcFuncs[i].name);
      break;
    }

    printf("\n*Выберите пункт меню: ");
    scanf("%9d", &menu_num);
    Util_ClearScanf();

    while (menu_num < 1 ||
           menu_num > (sizeof(CalcFuncs) / sizeof(CalcFuncs[0]))) {
      printf("\n*Нужно ввести число от "
             "1 до %d!\n",
             g_max_num);
      printf("*Выберите пункт меню: ");
      scanf("%1d", &menu_num);
      Util_ClearScanf();
    }
    printf("\n");

    CalcStdOut(menu_num);
  }
}

void SafeLibLink() {
  char name[256] = {0};
  int i = 1;

  while (1) {
    printf("#ВВЕДИТЕ НАЗВАНИЕ БИБЛИОТЕКИ ДЛЯ ПОДКЛЮЧЕНИЯ#\n"
           "#ИЛИ '@' ДЛЯ ОКОНЧАНИЯ ВВОДА#\n");

    scanf("%255s", name);
    Util_ClearScanf();
    if (name[0] == '@')
      break;

    handle = dlopen(name, RTLD_LAZY);
    if (handle == NULL) {
      printf("\n#НЕВЕРНОЕ НАЗВАНИЕ БИБЛИОТЕКИ#\n");
      continue;
    }

    func_name = dlsym(handle, "func_name");
    if (func_name == NULL) {
      printf("\n#ОТСУТСТВУЕТ ФУНКЦИЯ ДЛЯ ДОБАВЛЕНИЯ#\n");
      continue;
    } else {
      CalcFuncs[i].func = dlsym(handle, func_name);
      i++;
      handle = NULL;
      func_ptr = NULL;

      g_max_num = sizeof(CalcFuncs) / sizeof(CalcFuncs[0]);

      break;
    }
  }
}

int main() {
  SafeLibLink();

  CalcMenu();

  return 0;
}
