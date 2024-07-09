#include <dlfcn.h>
#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct CalcFuncsBook {
  char name[128];
  char expression[128];
  long (*func)(long, long);
};

static struct CalcFuncsBook *CalcFuncs = NULL;
static int g_max_num;

static void CalcExit() {
  free(CalcFuncs);
  exit(0);
}

static void Util_ClearScanf() {
  int c;
  while ((c = fgetc(stdin)) != EOF && c != '\n')
    ;
}

static void Util_SafeRealloc(int realloc_size) {
  CalcFuncs =
      reallocarray(CalcFuncs, realloc_size, sizeof(struct CalcFuncsBook));

  if (CalcFuncs == NULL)
    err(EXIT_FAILURE, "Realloc's NULL!\nLine: %d\n", __LINE__);
}

static long CalcStdIn(char letter) {

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
    } else {
      break;
    }

  } while (1);

  return number;
}

static void CalcStdOut(int calc_num) {
  long number_a, number_b;

  printf("*%d) ", calc_num);
  calc_num--;

  if (calc_num != g_max_num - 1) {

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

static void CalcMenu() {
  int menu_num;

  while (1) {
    printf("\n*Калькулятор\n\n");
    for (int i = 0; i < g_max_num; i++) {
      printf("%d) %s\n", i + 1, CalcFuncs[i].name);
    }

    printf("\n*Выберите пункт меню: ");
    scanf("%9d", &menu_num);
    Util_ClearScanf();

    while (menu_num < 1 || menu_num > g_max_num) {
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

static void AddMenuItem(char name[128], char expression[128],
                        long (*ptr)(long, long)) {
  g_max_num++;
  Util_SafeRealloc(g_max_num);

  strcpy(CalcFuncs[g_max_num - 1].name, name);
  strcpy(CalcFuncs[g_max_num - 1].expression, expression);

  CalcFuncs[g_max_num - 1].func = ptr;
}

static void SafeLibLink() {
  char std_str[256] = {0};

  void *handle;
  char *func_name, *expression_name, *expression;
  long (*func_ptr)(long, long);

  while (1) {
    printf("# ВВЕДИТЕ ПОЛНЫЙ ПУТЬ ДО БИБЛИОТЕКИ ДЛЯ ПОДКЛЮЧЕНИЯ #\n"
           "# ИЛИ '@' ДЛЯ ОКОНЧАНИЯ ВВОДА #\n");

    scanf("%255s", std_str);
    Util_ClearScanf();
    if (std_str[0] == '@') {
      AddMenuItem("Выход", "Выходим", NULL);
      break;
    }

    handle = dlopen(std_str, RTLD_LAZY);
    if (handle == NULL) {
      printf("\n! # ДАННАЯ БИБЛИОТЕКА НЕ НАЙДЕНА #\n\n");
    } else {
      func_name = dlsym(handle, "func_name");
      if (func_name == NULL) {
        printf("\n! # ОТСУТСТВУЕТ ССЫЛКА 'func_name'"
               " НА ФУНКЦИЮ ДЛЯ ДОБАВЛЕНИЯ #\n");
      } else {
        func_ptr = dlsym(handle, func_name);
        if (func_ptr == NULL) {
          printf("\n! # ОТСУТСТВУЕТ ФУНКЦИЯ ДЛЯ ДОБАВЛЕНИЯ #\n");
        } else {

          expression_name = dlsym(handle, "expression_name");
          if (expression_name == NULL) {
            printf("\n! # ОТСУТСТВУЕТ ПЕРЕМЕННАЯ 'expression_name'"
                   " ДЛЯ ДОБАВЛЕНИЯ #\n");
          } else {
            expression = dlsym(handle, "expression");
            if (expression == NULL) {
              printf("\n! # ОТСУТСТВУЕТ ПЕРЕМЕННАЯ 'expression'"
                     " ДЛЯ ДОБАВЛЕНИЯ #\n");
            } else {
              AddMenuItem(expression_name, expression, func_ptr);

              printf("# ДОБАВЛЕНИЕ ФУНКЦИИ %s ПРОШЛО УСПЕШНО #\n\n", func_name);

              expression_name = NULL;
              expression = NULL;
              handle = NULL;
              func_name = NULL;
              func_ptr = NULL;
            }
          }
        }
      }
    }
  }
}

int main() {
  SafeLibLink();

  CalcMenu();

  return 0;
}
