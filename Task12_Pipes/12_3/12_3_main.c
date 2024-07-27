#include <stdio.h>

#include "./12_3_command.h"

int
main ()
{
  printf ("\n*[ (не) Bash ]*\n\n");

  CommandPrepare ();

  printf ("\n*[ Bash ]*\n\n");

  return 0;
}