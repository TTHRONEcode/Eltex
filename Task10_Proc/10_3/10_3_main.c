#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./../10_ERRCHECKER.h"

int main() {
  int child_exit_status;
  char pathname[257] = {0}, command[257] = {0}, args[257] = {0};

  while (1) {
    printf("*Введите путь до команды: ");
    scanf("%256s", pathname);

    if (strcmp(pathname, "exit") == 0) {
      printf("*Выходим...\n");
      break;
    }

    printf("*Введите название самой программы: ");
    scanf("%256s", command);

    printf("*Введите агрументы для комманды: ");
    scanf("%256s", args);

    printf("*Выполнение программы: %s %s\n", pathname, args);

    if (D_ERRCHECK_fork == 0) { // CHILD
      if ((execl(pathname, command, args, NULL)) == -1) {
        perror("execl");
        exit(EXIT_FAILURE);
      }
    } else { // PARENT

      D_ERRCHECK_wait;
      child_exit_status = D_ERRCHECK_wait;
      printf("*Child exit status is %d.\n\n", WEXITSTATUS(child_exit_status));
    }
  }

  return 0;
}