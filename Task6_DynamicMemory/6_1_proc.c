#ifndef INCLUDE_PROC

#include <err.h>
#include <limits.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INCLUDE_PROC
#define STRUCT_ELEMENTS_ARRAY_SIZE 10

enum { ADD = 1, DELETE, SEARCH, PRINT_ALL, EXIT };

struct AbonentList {
  char name[STRUCT_ELEMENTS_ARRAY_SIZE];
  char second_name[STRUCT_ELEMENTS_ARRAY_SIZE];
  char tel[STRUCT_ELEMENTS_ARRAY_SIZE];
};

struct AbonentList *directory = NULL;
char *ptr_errno;

char g_buffer_name[STRUCT_ELEMENTS_ARRAY_SIZE + 1];
int g_free_directory, i, j;
bool g_was_changed, g_was_detected;

void Proc_ClearScanf() {
  int c;
  while ((c = fgetc(stdin)) != EOF && c != '\n')
    ;
}

void Proc_ClearBuffer() {
  for (i = 0; i < STRUCT_ELEMENTS_ARRAY_SIZE + 1; i++) {
    g_buffer_name[i] = 0;
  }
}

void Proc_SafeRealloc(int __realloc_size) {
  directory =
      reallocarray(directory, __realloc_size, sizeof(struct AbonentList));

  if (directory == NULL)
    err(EXIT_FAILURE, "Realloc's NULL!\nLine: %d\n", __LINE__);
  // проверка из man malloc
}

// Добавление абонента
void Proc_DirAdd() {
  printf("*%d) Добавление абонента\n", ADD);

  if (g_free_directory != -1 && g_free_directory != INT_MAX) {

    Proc_SafeRealloc(g_free_directory + 1);
    for (i = 0; i < STRUCT_ELEMENTS_ARRAY_SIZE; i++) {
      directory[g_free_directory].name[i] = 0;
      directory[g_free_directory].second_name[i] = 0;
      directory[g_free_directory].tel[i] = 0;
    }

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

    printf("*Абонент успешно добавлен!\n");
    g_free_directory++;

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
  Proc_ClearScanf();

  g_was_detected = false;
  for (i = 0; i < g_free_directory; i++) {

    g_was_changed = true;
    for (j = 0; j < STRUCT_ELEMENTS_ARRAY_SIZE; j++) {
      if (directory[i].name[j] != g_buffer_name[j]) {
        g_was_changed = false;
        break;
      }
    }

    if (g_was_changed) {
      for (j = i; j < STRUCT_ELEMENTS_ARRAY_SIZE; j++) {
        directory[i].name[j] = directory[g_free_directory - 1].name[j];
        directory[i].second_name[j] =
            directory[g_free_directory - 1].second_name[j];
        directory[i].tel[j] = directory[g_free_directory - 1].tel[j];
      }

      g_free_directory--;
      Proc_SafeRealloc(g_free_directory);
      g_was_detected = true;

      printf("*Абонент №%3i %s был успешно удален.\n", i + 1, g_buffer_name);
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

  g_was_detected = false;
  // printf("*Найденые абоненты с именем %s:\n", g_buffer_name);
  for (i = 0; i < g_free_directory; i++) {
    g_was_changed = true;
    for (j = 0; j < STRUCT_ELEMENTS_ARRAY_SIZE; j++) {
      if (directory[i].name[j] != g_buffer_name[j]) {
        g_was_changed = false;

        break;
      }
    }
    if (g_was_changed) {
      g_was_detected = true;
      printf("№%3i. %s %s, тел.: %s\n", i + 1, directory[i].name,
             directory[i].second_name, directory[i].tel);
    }
  }

  if (!g_was_detected)
    printf("*Абонентов с именем %s не найдено.\n", g_buffer_name);
}

//  Вывод всех записей
void Proc_DirPrintAll() {
  printf("*%d) Вывод всех записей:\n", PRINT_ALL);

  g_was_changed = false;

  for (i = 0; i < g_free_directory; i++) {
    if (directory[i].name[0] != 0) {
      g_was_changed = true;
      printf("№%3i. %s %s, тел.: %s\n", i + 1, directory[i].name,
             directory[i].second_name, directory[i].tel);
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
#endif