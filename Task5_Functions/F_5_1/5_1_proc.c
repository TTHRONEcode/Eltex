#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define STRUCT_SIZE 100
#define STRUCT_ELEMENTS_ARRAY_SIZE 10

enum { ADD = 1, DELETE, SEARCH, PRINT_ALL, EXIT };

struct AbonentList {
  char name[STRUCT_ELEMENTS_ARRAY_SIZE];
  char second_name[STRUCT_ELEMENTS_ARRAY_SIZE];
  char tel[STRUCT_ELEMENTS_ARRAY_SIZE];
};

struct AbonentList directory[STRUCT_SIZE];
char g_buffer_name[STRUCT_ELEMENTS_ARRAY_SIZE + 1];
int g_free_directory, g_i, g_j;
bool g_was_changed, g_was_detected;

void Proc_ClearScanf() {
  int c;
  while ((c = fgetc(stdin)) != EOF && c != '\n')
    ;
}

void Proc_ClearBuffer() {
  for (g_i = 0; g_i < STRUCT_ELEMENTS_ARRAY_SIZE + 1; g_i++) {
    g_buffer_name[g_i] = 0;
  }
}

void Proc_FindFreeSpace() {
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
}

// Добавление абонента
void Proc_DirAdd() {
  printf("*%d) Добавление абонента\n", ADD);

  if (g_free_directory != -1) {
    printf("*Введите имя абонента (%i символов): \n",
           STRUCT_ELEMENTS_ARRAY_SIZE);
    scanf("%10s", directory[g_free_directory].name);
    Proc_ClearScanf();

    printf("*Введите фамилию абонента (%i символов): \n",
           STRUCT_ELEMENTS_ARRAY_SIZE);
    scanf("%10s", directory[g_free_directory].second_name);
    Proc_ClearScanf();

    printf("*Введите телефон абонента (%i символов): \n",
           STRUCT_ELEMENTS_ARRAY_SIZE);
    scanf("%10s", directory[g_free_directory].tel);
    Proc_ClearScanf();

    Proc_FindFreeSpace();

    printf("*Абонент успешно добавлен!\n");
  } else {
    printf("*Невозможно добавить абонента: нет свободных полей.\n"
           "*Для добавления необходимо удалить лишнего абонента из "
           "справочника.\n");
  }
}

// Удаление абонента
void Proc_DirDelete() {
  printf("*%d) Удаление абонента\n", DELETE);
  printf("*Введите имя абонентов для удаления:\n");

  Proc_ClearBuffer();

  scanf("%10s", g_buffer_name);

  g_was_detected = false;
  for (g_i = 0; g_i < STRUCT_SIZE; g_i++) {

    g_was_changed = true;
    for (g_j = 0; g_j < STRUCT_ELEMENTS_ARRAY_SIZE; g_j++) {
      if (directory[g_i].name[g_j] != g_buffer_name[g_j]) {
        g_was_changed = false;
        break;
      }
    }

    if (g_was_changed) {
      for (g_j = 0; g_j < STRUCT_ELEMENTS_ARRAY_SIZE; g_j++) {
        directory[g_i].name[g_j] = 0;
        directory[g_i].second_name[g_j] = 0;
        directory[g_i].tel[g_j] = 0;
      }

      g_was_detected = true;

      printf("*Абонент №%3i %s был успешно удален.\n", g_i + 1, g_buffer_name);

      for (g_j = 0; g_j < STRUCT_SIZE; g_j++) {
        if (directory[g_j].name[0] == 0) {
          g_free_directory = g_j;
          break;
        }
      }
    }
  }

  if (!g_was_detected)
    printf("*Абонентов с именем %s не найдено.\n", g_buffer_name);
}

//  Поиск абонентов по имени
void Proc_DirSearch() {
  printf("*%d) Поиск абонентов по имени\n", SEARCH);
  printf("*Введите имя абонентов для поиска: ");

  Proc_ClearBuffer();

  scanf("%10s", g_buffer_name);
  Proc_ClearScanf();

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

  if (!g_was_detected)
    printf("*А-н нет, абонентов с именем %s не найдено.\n", g_buffer_name);
}

//  Вывод всех записей
void Proc_DirPrintAll() {
  printf("*%d) Вывод всех записей:\n", PRINT_ALL);

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

// Выход
void Proc_DirExit() {
  printf("*%d) Выход\n*Выходим...\n", EXIT);
  exit(0);
}