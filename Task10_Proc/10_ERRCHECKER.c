#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t ERRCHECK_fork(const char *FILE_NAME, const char *func, int LINE) {
  pid_t f = fork();
  if (f == -1)
    err(EXIT_FAILURE, "fork, %s, %s, %d\n", FILE_NAME, func, LINE);

  return f;
}

int ERRCHECK_wait(const char *FILE_NAME, const char *func, int LINE) {
  int w;
  wait(&w);
  if (w == -1)
    err(EXIT_FAILURE, "wait, %s, %s, %d\n", FILE_NAME, func, LINE);

  return w;
}