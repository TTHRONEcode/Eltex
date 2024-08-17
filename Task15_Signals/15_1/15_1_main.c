#include <complex.h>
#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include <unistd.h>

void
SigHandler (int sig_num, siginfo_t *info __attribute__ ((unused)),
            void *args __attribute__ ((unused)))
{
  printf ("*Пришёл сигнал №%d: SIGUSR1*\n", sig_num);
}

int
main ()
{
  struct sigaction sig_handler;
  sigset_t sig_set;

  sigemptyset (&sig_set);
  sigaddset (&sig_set, SIGUSR1);
  sig_handler.sa_sigaction = SigHandler;
  sig_handler.sa_mask = sig_set;
  sig_handler.sa_flags = 0;

  if (sigaction (SIGUSR1, &sig_handler, NULL) < 0)
    {
      err (EXIT_FAILURE, "sigaction\n");
    }

  while (1)
    {
      sleep (1);
    }

  return 0;
}