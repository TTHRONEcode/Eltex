#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./../../Task10_Proc/10_ERRCHECKER.h"
// #include "./../../Task10_Proc/10_printf_proc_p_pid.h"

int
main ()
{
  int pipe_des[2];
  char *str_wr = "Hi!";
  char str_r[3] = { 0 };

  if (pipe (pipe_des) == -1)
    {
      perror ("pipe");
      exit (EXIT_FAILURE);
    }

  if (D_ERRCHECK_fork == 0)
    {
      close (pipe_des[1]);
      read (pipe_des[0], str_r, strlen (str_wr) * sizeof (char));
      printf ("%s\n", str_r);
      exit (EXIT_SUCCESS);
    }
  else
    {
      close (pipe_des[0]);
      write (pipe_des[1], str_wr, strlen (str_wr) * sizeof (char));
      D_ERRCHECK_wait;
    }

  exit (EXIT_SUCCESS);
}
