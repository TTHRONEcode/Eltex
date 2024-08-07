// Написать программу абонентский справочник. Список абонентов
// представляет собой статический массив (100 элементов) из структур
// следующего в вида:
// struct abonent {
// char name[10];
// char second_name[10];
// char tel[10];
// };
// При запуске на экран выводится текстовое меню:
// 1) Добавить абонента
// 2) Удалить абонента
// 3) Поиск абонентов по имени
// 4) Вывод всех записей
// 5) Выход
// и пользователю предлагается ввести пункт меня с клавиатуры. Добавление
// абонента в массив реализуется простым заполнением свободной структуры,
// при выходе за 100 абонентов уведомить пользователя о переполнении
// справочника и не позволять больше добавлять абонентов. При удалении
// структура заполняется нулями. При поиске пользователь вводит с клавиатуры
// имя абонентов и на экран выводится список всех абонентов с таким же именем.
// Программа продолжает выполняться пока пользователь не введет пункт 5.

#include <stdbool.h>
#include <stdio.h>

#define STRUCT_SIZE 100
#define STRUCT_ELEMENTS_ARRAY_SIZE 10

void ClearScanf() {
  int c;
  while ((c = fgetc(stdin)) != EOF && c != '\n')
    ;
}

struct abonent {
  char name[STRUCT_ELEMENTS_ARRAY_SIZE];
  char second_name[STRUCT_ELEMENTS_ARRAY_SIZE];
  char tel[STRUCT_ELEMENTS_ARRAY_SIZE];
};

int main() {
  struct abonent directory[STRUCT_SIZE] = {0};
  char buffer_name[STRUCT_ELEMENTS_ARRAY_SIZE + 1] = {0};

  int free_directory = 0, menu_num = 0, i, j;
  bool was_changed, was_detected;

  while (menu_num != 5) {
    menu_num = -1;
    printf("\n*Абонентский справочник*\n*Меню:\n"
           "1) Добавить абонента\n"
           "2) Удалить абонента\n"
           "3) Поиск абонентов по имени\n"
           "4) Вывод всех записей\n"
           "5) Выход\n");
    printf("*Выберите пункт меню: ");
    scanf("%1d", &menu_num);
    ClearScanf();

    while (menu_num < 0 || menu_num > 5) {
      printf("\n*Нужно ввести число от 1 до 5!\n");
      printf("*Выберите пункт меню: ");
      scanf("%1d", &menu_num);
      ClearScanf();
    }
    printf("\n");

    switch (menu_num) {
    case 1:
      printf("*1) Добавление абонента\n");

      if (free_directory != -1) {
        printf("*Введите имя абонента (%i символов): \n",
               STRUCT_ELEMENTS_ARRAY_SIZE);
        scanf("%10s", directory[free_directory].name);
        ClearScanf();

        printf("*Введите фамилию абонента (%i символов): \n",
               STRUCT_ELEMENTS_ARRAY_SIZE);
        scanf("%10s", directory[free_directory].second_name);
        ClearScanf();

        printf("*Введите телефон абонента (%i символов): \n",
               STRUCT_ELEMENTS_ARRAY_SIZE);
        scanf("%10s", directory[free_directory].tel);
        ClearScanf();

        was_changed = false;
        for (j = 0; j < STRUCT_SIZE; j++) {
          if (directory[j].name[0] == 0) {
            free_directory = j;
            was_changed = true;
            break;
          }
        }

        if (!was_changed)
          free_directory = -1;

        printf("*Абонент успешно добавлен!\n");
      } else {
        printf("*Невозможно добавить абонента: нет свободных полей.\n"
               "*Для добавления необходимо удалить лишнего абонента из "
               "справочника.\n");
      }
      break;

    case 2:
      printf("*2) Удаление абонента\n");

      printf("*Введите имя абонентов для удаления:\n");

      for (i = 0; i < STRUCT_ELEMENTS_ARRAY_SIZE + 1; i++) {
        buffer_name[i] = 0;
      }

      scanf("%10s", buffer_name);
      ClearScanf();

      was_detected = false;
      for (i = 0; i < STRUCT_SIZE; i++) {

        was_changed = true;
        for (j = 0; j < STRUCT_ELEMENTS_ARRAY_SIZE; j++) {
          if (directory[i].name[j] != buffer_name[j]) {
            was_changed = false;
            break;
          }
        }

        if (was_changed) {
          for (j = 0; j < STRUCT_ELEMENTS_ARRAY_SIZE; j++) {
            directory[i].name[j] = 0;
            directory[i].second_name[j] = 0;
            directory[i].tel[j] = 0;
          }

          was_detected = true;

          printf("*Абонент №%3i %s был успешно удален.\n", i + 1, buffer_name);

          for (j = 0; j < STRUCT_SIZE; j++) {
            if (directory[j].name[0] == 0) {
              free_directory = j;
              break;
            }
          }
        }
      }

      if (!was_detected)
        printf("*Абонентов с именем %s не найдено.\n", buffer_name);
      break;

    case 3:
      printf("*3) Поиск абонентов по имени\n");
      printf("*Введите имя абонентов для поиска: ");
      for (i = 0; i < STRUCT_ELEMENTS_ARRAY_SIZE + 1; i++) {
        buffer_name[i] = 0;
      }
      scanf("%10s", buffer_name);
      ClearScanf();

      was_detected = false;
      printf("*Найденые абоненты с именем %s:\n", buffer_name);
      for (i = 0; i < STRUCT_SIZE; i++) {
        was_changed = true;
        for (j = 0; j < STRUCT_ELEMENTS_ARRAY_SIZE; j++) {
          if (directory[i].name[j] != buffer_name[j]) {
            was_changed = false;
            break;
          }
        }
        if (was_changed) {
          was_detected = true;
          printf("№%3i. %s %s, тел.: %s\n", i + 1, directory[i].name,
                 directory[i].second_name, directory[i].tel);
        }
      }
      if (!was_detected)
        printf("*А-н нет, абонентов с именем %s не найдено.\n", buffer_name);

      break;

    case 4:
      printf("*4) Вывод всех записей:\n");
      was_changed = false;

      for (i = 0; i < STRUCT_SIZE; i++) {
        if (directory[i].name[0] != 0) {
          was_changed = true;
          printf("№%3i. %s %s, тел.: %s\n", i + 1, directory[i].name,
                 directory[i].second_name, directory[i].tel);
        }
      }
      if (!was_changed)
        printf("*Список пуст. Самое время добавить абонента!\n");
      break;

    default:
      printf("*5) Выход\n*Выходим...\n");
      break;
    }
  }

  return 0;
}