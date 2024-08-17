#include <complex.h>
#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include <unistd.h>

int
main ()
{
  sigset_t sig_set;
  int sig_num;

  sigemptyset (&sig_set);
  sigaddset (&sig_set, SIGUSR1);

  if (sigprocmask (SIG_BLOCK, &sig_set, NULL))
    {
      err (EXIT_FAILURE, "sigaction\n");
    }

  while (1)
    {
      sigwait (&sig_set, &sig_num);
      printf ("*Ну, дождались... (сигнал №%d)*\n", sig_num);
    }

  return 0;
}