// 1. Переписать абонентский справочник с использованием функций.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define STRUCT_SIZE 100
#define STRUCT_ELEMENTS_ARRAY_SIZE 10

struct AbonentList {
  char name[STRUCT_ELEMENTS_ARRAY_SIZE + 1];
  char second_name[STRUCT_ELEMENTS_ARRAY_SIZE + 1];
  char tel[STRUCT_ELEMENTS_ARRAY_SIZE + 1];
};

static struct AbonentList directory[STRUCT_SIZE];

char buffer_name[STRUCT_ELEMENTS_ARRAY_SIZE + 1];
int free_directory, menu_num, i, j;
bool was_changed, was_deleted;

void UtilityClearScanf() {
  int c;
  while ((c = fgetc(stdin)) != EOF && c != '\n')
    ;
}

void UtilityClearBuffer() {
  for (i = 0; i < STRUCT_ELEMENTS_ARRAY_SIZE + 1; i++) {
    buffer_name[i] = 0;
  }
}

// 1) Добавление абонента
void DirectoryAdd() {
  printf("*1) Добавление абонента\n");

  if (free_directory != -1) {
    printf("*Введите имя абонента (%i символов): \n",
           STRUCT_ELEMENTS_ARRAY_SIZE);
    scanf("%10s", directory[free_directory].name);
    UtilityClearScanf();

    printf("*Введите фамилию абонента (%i символов): \n",
           STRUCT_ELEMENTS_ARRAY_SIZE);
    scanf("%10s", directory[free_directory].second_name);
    UtilityClearScanf();

    printf("*Введите телефон абонента (%i символов): \n",
           STRUCT_ELEMENTS_ARRAY_SIZE);
    scanf("%10s", directory[free_directory].tel);
    UtilityClearScanf();

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
}

// 2) Удаление абонента
void DirectoryDelete() {
  printf("*2) Удаление абонента\n");
  printf("*Введите имя абонентов для удаления:\n");

  UtilityClearBuffer();

  scanf("%10s", buffer_name);
  UtilityClearScanf();

  was_deleted = false;
  for (i = 0; i < STRUCT_SIZE; i++) {

    was_changed = true;
    for (j = 0; j < STRUCT_ELEMENTS_ARRAY_SIZE; j++) {
      if (directory[i].name[j] != buffer_name[j]) {
        was_changed = false;
        break;
      }
    }

    if (was_changed) {
      for (j = 0; j < STRUCT_ELEMENTS_ARRAY_SIZE + 1; j++) {
        directory[i].name[j] = 0;
        directory[i].second_name[j] = 0;
        directory[i].tel[j] = 0;
      }

      was_deleted = true;

      printf("*Абонент №%3i %s был успешно удален.\n", i + 1, buffer_name);

      for (j = 0; j < STRUCT_SIZE; j++) {
        if (directory[j].name[0] == 0) {
          free_directory = j;
          break;
        }
      }
    }
  }

  if (!was_deleted)
    printf("*Абонентов с именем %s не найдено.\n", buffer_name);
}

// 3) Поиск абонентов по имени
void DirectoryFind() {
  printf("*3) Поиск абонентов по имени\n");
  printf("*Введите имя абонентов для поиска: ");

  UtilityClearBuffer();

  scanf("%10s", buffer_name);
  UtilityClearScanf();

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
      printf("№%3i. %s %s, тел.: %s\n", i + 1, directory[i].name,
             directory[i].second_name, directory[i].tel);
    }
  }
}

// 4) Вывод всех записей
void DirectoryPrint() {
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
}

// 5) Выход
void DirectoryExit() {
  printf("*5) Выход\n*Выходим...\n");
  exit(0);
}

// Меню
void DirectoryMenu() {
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
    UtilityClearScanf();

    while (menu_num < 0 || menu_num > 5) {
      printf("\n*Нужно ввести число от 1 до 5!\n");
      printf("*Выберите пункт меню: ");
      scanf("%1d", &menu_num);
      UtilityClearScanf();
    }
    printf("\n");

    switch (menu_num) {
    case 1:
      DirectoryAdd();
      break;

    case 2:
      DirectoryDelete();
      break;

    case 3:
      DirectoryFind();
      break;

    case 4:
      DirectoryPrint();
      break;

    default:
      DirectoryExit();
      break;
    }
  }
}

int main() {
  DirectoryMenu();

  return 0;
}