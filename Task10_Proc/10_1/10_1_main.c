#include <stdio.h>
#include <stdlib.h>

#include "./../10_ERRCHECKER.h"
#include "./../10_printf_proc_p_pid.h"

int main() {
  int child_exit_status;

  if (D_ERRCHECK_fork == 0) {
    PrintfProcPPID("CHILD");
    exit(228);
  } else {
    PrintfProcPPID("PARENT");

    child_exit_status = D_ERRCHECK_wait;
    printf("Child exit status is %d.\n", WEXITSTATUS(child_exit_status));
  }

  return 0;
}