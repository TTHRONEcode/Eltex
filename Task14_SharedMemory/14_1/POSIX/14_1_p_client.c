#include <err.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "./14_1_p_header.h"

void
CallError (char *err_str)
{
  err (EXIT_FAILURE, "%s", err_str);
}

void
CheckError (int err_n, char *err_str)
{
  if (err_n < 0)
    {
      CallError (err_str);
    }
}

int
main ()
{
  int shared_fd = 0;
  char *ret_mmap = NULL;
  sem_t *ret_sem_open = NULL;

  if ((ret_sem_open = sem_open (k_semaphore_name, O_RDWR)) == SEM_FAILED)
    {
      CallError ("sem_open");
    }

  CheckError (shared_fd = shm_open (k_server_name, O_RDWR, 0666), "shm_open");

  //CheckError (ftruncate (shared_fd, 8), "ftrucate");

  if ((ret_mmap
       = mmap (NULL, 8, PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0))
      == (void *)-1)
    {
      CallError ("mmap");
    }

  CheckError (printf ("%s\n", ret_mmap), "printf");

  strcpy (ret_mmap, "Hello!");

  CheckError (sem_post (ret_sem_open), "sem_post");

  CheckError (sem_close (ret_sem_open), "sem_close");

  CheckError (munmap (ret_mmap, 8), "ret_munmap");

  return 0;
}
