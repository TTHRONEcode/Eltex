#include <sys/types.h>
#include <unistd.h>

#include "./../10_ERRCHECKER.h"
#include "./../10_printf_proc_p_pid.h"

pid_t fork_pids[5];
int child_exit_status;

void ProcCreate() {
  if ((fork_pids[0] = D_ERRCHECK_fork) == 0) { // PROC 1
    PrintfProcPPID("PROC 1");
    if ((fork_pids[2] = D_ERRCHECK_fork) == 0) { // PROC 3
      PrintfProcPPID("PROC 3");
    } else {                                       // PROC 1
      if ((fork_pids[3] = D_ERRCHECK_fork) == 0) { // PROC 4
        PrintfProcPPID("PROC 4");
      } else { // PROC 1
        D_ERRCHECK_wait;
        D_ERRCHECK_wait;
      }
    }
  } else {
    if ((fork_pids[1] = D_ERRCHECK_fork) == 0) { // PROC 2
      PrintfProcPPID("PROC 2");
      if ((fork_pids[4] = D_ERRCHECK_fork) == 0) { // PROC 5
        PrintfProcPPID("PROC 5");
      } else { // PROC 2
        D_ERRCHECK_wait;
      }
    } else { // PARENT
      D_ERRCHECK_wait;
      PrintfProcPPID("PARENT");
    }
  }
}