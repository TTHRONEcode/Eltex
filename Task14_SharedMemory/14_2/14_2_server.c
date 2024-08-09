#include <err.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "./14_2_header.h"

int shm_fds[SHM_FD_ELEMENTS_COUNT_];
int *shm_clients_amount;
sem_t *semaphore_main, *semaphore_exit;

static void
SetSomeSharedMem ()
{
  ftruncate (shm_fds[SHM_FD_CLIENTS_AMOUNT], sizeof (int));
  shm_clients_amount
      = (int *)mmap (NULL, sizeof (int), PROT_READ | PROT_WRITE, MAP_SHARED,
                     shm_fds[SHM_FD_CLIENTS_AMOUNT], 0);

  (*shm_clients_amount) = 0;

  ftruncate (shm_fds[SHM_FD_MESSAGES_AMOUNT], sizeof (int));
}

static void
GetSharedMemFdsAndSemaphore ()
{
  for (int i = 0; i < SHM_FD_ELEMENTS_COUNT_; i++)
    {
      shm_fds[i] = shm_open (k_shm_names[i], O_CREAT | O_RDWR, 0666);
    }

  semaphore_main
      = sem_open (k_sem_names[SEMAPHORE_MAIN], O_CREAT | O_RDWR, 0666, 0);
  semaphore_exit
      = sem_open (k_sem_names[SEMAPHORE_EXIT], O_CREAT | O_RDWR, 0666, 0);

  sem_post (semaphore_main);
}

static void
ServerLogic ()
{
  GetSharedMemFdsAndSemaphore ();

  SetSomeSharedMem ();

  printf ("*Server get started*\n"
          "*Press any key to exit...\n");
  getchar ();

  sem_post (semaphore_exit);

  printf ("*Waiting until all clients are disconnected*\n");

  while ((*shm_clients_amount) != 0)
    ;

  printf ("*All clients are disconnected*\n"
          "*Prepare to exit*\n");

  sem_close (semaphore_main);
  sem_close (semaphore_exit);
  sem_unlink (k_sem_names[SEMAPHORE_MAIN]);
  sem_unlink (k_sem_names[SEMAPHORE_EXIT]);

  munmap (shm_clients_amount, sizeof (int));

  for (int i = 0; i < SHM_FD_ELEMENTS_COUNT_; i++)
    {
      shm_unlink (k_shm_names[i]);
    }

  printf ("*Bye-bye...\n");
}

int
main ()
{
  ServerLogic ();

  return 0;
}