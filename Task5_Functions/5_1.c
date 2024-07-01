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

struct AbonentList directory[STRUCT_SIZE];
char g_buffer_name[STRUCT_ELEMENTS_ARRAY_SIZE + 1];
int g_free_directory, g_menu_num, g_i, g_j;
bool g_was_changed, g_was_deleted;

void UtilityClearScanf() {
  int c;
  while ((c = fgetc(stdin)) != EOF && c != '\n')
    ;
}

void UtilityClearBuffer() {
  for (g_i = 0; g_i < STRUCT_ELEMENTS_ARRAY_SIZE + 1; g_i++) {
    g_buffer_name[g_i] = 0;
  }
}

// 1) Добавление абонента
void DirectoryAdd() {
  printf("*1) Добавление абонента\n");

  if (g_free_directory != -1) {
    printf("*Введите имя абонента (%i символов): \n",
           STRUCT_ELEMENTS_ARRAY_SIZE);
    scanf("%10s", directory[g_free_directory].name);
    UtilityClearScanf();

    printf("*Введите фамилию абонента (%i символов): \n",
           STRUCT_ELEMENTS_ARRAY_SIZE);
    scanf("%10s", directory[g_free_directory].second_name);
    UtilityClearScanf();

    printf("*Введите телефон абонента (%i символов): \n",
           STRUCT_ELEMENTS_ARRAY_SIZE);
    scanf("%10s", directory[g_free_directory].tel);
    UtilityClearScanf();

    g_was_changed = false;
    for (g_j = 0; g_j < STRUCT_SIZE; g_j++) {
      if (directory[g_j].name[0] == 0) {
        g_free_directory = g_j;
        g_was_changed = true;
        break;
      }
    }

    if (!g_was_changed)
      g_free_directory = -1;

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

  scanf("%10s", g_buffer_name);
  UtilityClearScanf();

  g_was_deleted = false;
  for (g_i = 0; g_i < STRUCT_SIZE; g_i++) {

    g_was_changed = true;
    for (g_j = 0; g_j < STRUCT_ELEMENTS_ARRAY_SIZE; g_j++) {
      if (directory[g_i].name[g_j] != g_buffer_name[g_j]) {
        g_was_changed = false;
        break;
      }
    }

    if (g_was_changed) {
      for (g_j = 0; g_j < STRUCT_ELEMENTS_ARRAY_SIZE + 1; g_j++) {
        directory[g_i].name[g_j] = 0;
        directory[g_i].second_name[g_j] = 0;
        directory[g_i].tel[g_j] = 0;
      }

      g_was_deleted = true;

      printf("*Абонент №%3i %s был успешно удален.\n", g_i + 1, g_buffer_name);

      for (g_j = 0; g_j < STRUCT_SIZE; g_j++) {
        if (directory[g_j].name[0] == 0) {
          g_free_directory = g_j;
          break;
        }
      }
    }
  }

  if (!g_was_deleted)
    printf("*Абонентов с именем %s не найдено.\n", g_buffer_name);
}

// 3) Поиск абонентов по имени
void DirectoryFind() {
  printf("*3) Поиск абонентов по имени\n");
  printf("*Введите имя абонентов для поиска: ");

  UtilityClearBuffer();

  scanf("%10s", g_buffer_name);
  UtilityClearScanf();

  printf("*Найденые абоненты с именем %s:\n", g_buffer_name);
  for (g_i = 0; g_i < STRUCT_SIZE; g_i++) {
    g_was_changed = true;
    for (g_j = 0; g_j < STRUCT_ELEMENTS_ARRAY_SIZE; g_j++) {
      if (directory[g_i].name[g_j] != g_buffer_name[g_j]) {
        g_was_changed = false;
        break;
      }
    }
    if (g_was_changed) {
      printf("№%3i. %s %s, тел.: %s\n", g_i + 1, directory[g_i].name,
             directory[g_i].second_name, directory[g_i].tel);
    }
  }
}

// 4) Вывод всех записей
void DirectoryPrint() {
  printf("*4) Вывод всех записей:\n");
  g_was_changed = false;

  for (g_i = 0; g_i < STRUCT_SIZE; g_i++) {
    if (directory[g_i].name[0] != 0) {
      g_was_changed = true;
      printf("№%3i. %s %s, тел.: %s\n", g_i + 1, directory[g_i].name,
             directory[g_i].second_name, directory[g_i].tel);
    }
  }
  if (!g_was_changed)
    printf("*Список пуст. Самое время добавить абонента!\n");
}

// 5) Выход
void DirectoryExit() {
  printf("*5) Выход\n*Выходим...\n");
  exit(0);
}

// Меню
void DirectoryMenu() {
  while (g_menu_num != 5) {
    g_menu_num = -1;
    printf("\n*Абонентский справочник*\n*Меню:\n"
           "1) Добавить абонента\n"
           "2) Удалить абонента\n"
           "3) Поиск абонентов по имени\n"
           "4) Вывод всех записей\n"
           "5) Выход\n");
    printf("*Выберите пункт меню: ");
    scanf("%1d", &g_menu_num);
    UtilityClearScanf();

    while (g_menu_num < 0 || g_menu_num > 5) {
      printf("\n*Нужно ввести число от 1 до 5!\n");
      printf("*Выберите пункт меню: ");
      scanf("%1d", &g_menu_num);
      UtilityClearScanf();
    }
    printf("\n");

    switch (g_menu_num) {
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