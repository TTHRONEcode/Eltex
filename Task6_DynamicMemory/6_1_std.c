#include "6_1_proc.c"

int g_menu_num; //

// Меню
void Std_DirectoryMenu() {
  while (g_menu_num != EXIT) {
    g_menu_num = -1;
    printf("\n*Абонентский справочник*\n*Меню:\n"
           "%d) Добавить абонента\n"
           "%d) Удалить абонента\n"
           "%d) Поиск абонентов по имени\n"
           "%d) Вывод всех записей\n"
           "%d) Выход\n",
           ADD, DELETE, SEARCH, PRINT_ALL, EXIT);
    printf("*Выберите пункт меню: ");
    scanf("%1d", &g_menu_num);
    Proc_ClearScanf();

    while (g_menu_num < ADD || g_menu_num > EXIT) {
      printf("\n*Нужно ввести число от 1 до 5!\n");
      printf("*Выберите пункт меню: ");
      scanf("%1d", &g_menu_num);
      Proc_ClearScanf();
    }
    printf("\n");

    switch (g_menu_num) {
    case ADD:
      Proc_DirAdd();
      break;

    case DELETE:
      Proc_DirDelete();
      break;

    case SEARCH:
      Proc_DirSearch();
      break;

    case PRINT_ALL:
      Proc_DirPrintAll();
      break;

    default:
      Proc_DirExit();
      break;
    }
  }
}
