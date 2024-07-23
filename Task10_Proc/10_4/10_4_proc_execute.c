#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./../10_ERRCHECKER.h"

void ProcExecute(char *pathname, char *name) {
  int child_exit_status = 0;
  if (D_ERRCHECK_fork == 0) { // CHILD
    if ((execl(pathname, name, NULL)) == -1) {
      perror("execl");
      printf("\n %s --- %s \n", pathname, name);
      exit(EXIT_FAILURE);
    }
  } else { // PARENT
    D_ERRCHECK_wait;
    child_exit_status = D_ERRCHECK_wait;
    printf("*Child exit status is %d.\n\n", WEXITSTATUS(child_exit_status));

    printf("*Нажмите любую клавишу чтобы продолжить*\n");
    getchar();
  }
}