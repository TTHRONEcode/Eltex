#include <stdio.h>
#include <unistd.h>

void PrintfProcPPID(char *proc_name) {
  printf("%s: pid = %d, ppid = %d\n", proc_name, getpid(), getppid());
  fflush(NULL);
}