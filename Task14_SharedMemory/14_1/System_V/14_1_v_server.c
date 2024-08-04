#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "./14_1_v_header.h"

void
CheckError (int err_n, char *err_str)
{
  if (err_n < 0)
    {
      err (EXIT_FAILURE, "%s", err_str);
    }
}

int
main ()
{
  key_t key = 0, sem_key = 0;

  int shared_mem_id = 0, sem_id = 0, sem_op_id = 0, sem_control = 0,
      shared_detach = 0, shared_control = 0;

  CheckError (key = ftok (k_key_pathname, 1), "ftok_1");
  CheckError (sem_key = ftok (k_key_pathname, 2), "ftok_2");

  CheckError (shared_mem_id = shmget (key, sizeof (char *), IPC_CREAT | 0666),
              TO_STRING (shmget));

  CheckError (sem_id = semget (sem_key, 1, IPC_CREAT | 0666),
              TO_STRING (semget));

  char *shared_char_ptr = (char *)shmat (shared_mem_id, 0, 0);
  if (shared_char_ptr == (void *)-1)
    {
      err (EXIT_FAILURE, TO_STRING (shared_attach));
    }

  strncpy (shared_char_ptr, "Hi!", strlen ("Hi!"));

  CheckError (sem_op_id = semop (sem_id, lock, 2), TO_STRING (semop));
  CheckError (sem_op_id = semop (sem_id, lock, 2), TO_STRING (semop));

  CheckError (printf ("%s\n", shared_char_ptr), "printf");

  CheckError (sem_op_id = semop (sem_id, &unlock, 1), TO_STRING (semop));
  CheckError (sem_op_id = semop (sem_id, &unlock, 1), TO_STRING (semop));

  CheckError (sem_control = semctl (sem_id, IPC_RMID, 0), TO_STRING (semctl));

  CheckError (shared_detach = shmdt (shared_char_ptr),
              TO_STRING (shared_detach));

  CheckError (shared_control = shmctl (shared_mem_id, IPC_RMID, 0),
              TO_STRING (shared_control));

  return 0;
}