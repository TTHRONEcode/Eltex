#include <err.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "./14_2_graphic.h"
#include "14_2_header.h"

#define _CLIENTS_ID_CUR_POS                                                   \
  shm_clients_id + (STRING_SIZE_MAX * (*shm_clients_amount - 1))

// enum define
#define SHM_MANIP_ELEMENTS_COUNT_ 3
typedef enum
{
  SHM_MANIP_CLIENT_ENTER,
  SHM_MANIP_CLIENT_EXIT,
  SHM_MANIP_MSG_WRITE,
} ShmManipulationModes;

// init variables
const unsigned int k_msecs = 250;

int shm_fds[SHM_FD_ELEMENTS_COUNT_];
int *shm_clients_amount = NULL, *shm_messages_amount = NULL;
char *shm_clients_id, *shm_message_str, *shm_message_client_id;

char this_client_id[STRING_SIZE_MAX + 1];

sem_t *semaphore_main, *semaphore_exit;

pthread_t threads[2];
typedef enum
{
  THREAD_WRITER,
  THREAD_READER
} ThreadsNames;

// functions prototypes
static void ShmFtruncateAndMmapFor (int __shm_fd_num);
static void ConnectAllSharedMemToProgramm ();
static void GenerateUniqId (char *__name_str);
// code itself

// ptr_1 == uniq id
//
// ptr_1 & ptr_2 == message string & message client id
static void *
SharedMemoryManipulation (int manip_mode, char *char_ptr_1, char *char_ptr_2)
{
  sem_wait (semaphore_main);

  switch (manip_mode)
    {
    case SHM_MANIP_CLIENT_ENTER:
      (*shm_clients_amount)++;

      ConnectAllSharedMemToProgramm ();

      GenerateUniqId (char_ptr_1);

      break; //

    case SHM_MANIP_MSG_WRITE:
      (*shm_messages_amount)++;

      ConnectAllSharedMemToProgramm ();

      strncpy (shm_message_str
                   + ((*shm_messages_amount - 1) * STRING_SIZE_MAX),
               char_ptr_1, strlen (char_ptr_1));
      strncpy (shm_message_client_id
                   + ((*shm_messages_amount - 1) * STRING_SIZE_MAX),
               char_ptr_2, strlen (char_ptr_2));

      break; //

    case SHM_MANIP_CLIENT_EXIT:
      for (int i = 0; i < (*shm_clients_amount); i++)
        {
          // find self in ids array
          if (strcmp (shm_clients_id + (STRING_SIZE_MAX * i), this_client_id)
              == 0)
            {
              // проверяем чтобы в strncpy не попадали одинаковые указатели, во
              // избежание непонятного поведения
              if (strcmp (
                      shm_clients_id + (STRING_SIZE_MAX * i),
                      shm_clients_id
                          + (STRING_SIZE_MAX * ((*shm_clients_amount) - 1)))
                  != 0)
                {
                  // copy (last id) to (current id)
                  strncpy ((shm_clients_id + (STRING_SIZE_MAX * i)),
                           (shm_clients_id
                            + (STRING_SIZE_MAX * ((*shm_clients_amount) - 1))),
                           (strlen (shm_clients_id
                                    + (STRING_SIZE_MAX
                                       * ((*shm_clients_amount) - 1)))));
                }
            }
        }

      (*shm_clients_amount)--;

      SignalGraphicExit ();

      sem_post (semaphore_main);

      sem_close (semaphore_main);
      sem_close (semaphore_exit);

      exit (EXIT_SUCCESS);

      return NULL;

      break; //
    }

  sem_post (semaphore_main);

  return NULL;
}

static void
GenerateUniqId (char *name_str)
{
  time_t time_not_str = time (NULL);
  char *time_to_str = asctime (gmtime (&time_not_str));

  strncpy (this_client_id, name_str, strlen (name_str));
  strcat (this_client_id, "\n");
  strncat (this_client_id, time_to_str, strlen (time_to_str));

  strncpy (_CLIENTS_ID_CUR_POS, this_client_id, strlen (this_client_id));
}

static void *
ThreadWriter ()
{
  char *buffer_input_handler = NULL;

  SharedMemoryManipulation (SHM_MANIP_CLIENT_ENTER,
                            InputHandler (INPUT_MODE_LOGIN), NULL);

  // get input while user don't pressed F10
  while (1)
    {
      if ((buffer_input_handler = InputHandler (INPUT_MODE_MESSAGE))
          == (void *)-1)
        break; // exit condition

      SharedMemoryManipulation (SHM_MANIP_MSG_WRITE, buffer_input_handler,
                                this_client_id);
    }

  return NULL;
}

static void *
ThreadReader ()
{
  int past_client_amount = 0, past_messages_amount = 0;
  int sem_val = 0;

  while (1)
    {
      if (usleep (k_msecs) == -1)
        err (EXIT_FAILURE, "usleep %d", __LINE__);

      sem_getvalue (semaphore_exit, &sem_val);

      if (sem_val == 1)
        {
          pthread_cancel (threads[THREAD_WRITER]);

          return NULL;
        }
      sem_wait (semaphore_main);
      if (past_client_amount != *shm_clients_amount)
        {
          past_client_amount = *shm_clients_amount;
          ConnectAllSharedMemToProgramm ();

          OutputStringArrayToWindowNum (NULL, shm_clients_id, NULL,
                                        *shm_clients_amount,
                                        WND_RIGHT_CLIENTS_ID);
        }

      if (past_messages_amount != *shm_messages_amount)
        {

          past_messages_amount = *shm_messages_amount;
          ConnectAllSharedMemToProgramm ();

          OutputStringArrayToWindowNum (
              this_client_id, shm_message_str, shm_message_client_id,
              *shm_messages_amount, WND_LEFT_MSGS_OF_CLIENTS);
        }
      sem_post (semaphore_main);
    }

  return NULL;
}

static void
ThreadsHandler ()
{
  int *thread_ext_status;

  pthread_create (&threads[THREAD_WRITER], NULL, ThreadWriter, NULL);
  pthread_create (&threads[THREAD_READER], NULL, ThreadReader, NULL);

  // if user pressed F10 OR server post the semaphore_exit...

  pthread_join (threads[THREAD_WRITER], (void **)&thread_ext_status);

  pthread_cancel (threads[THREAD_READER]);
  pthread_join (threads[THREAD_READER], (void **)&thread_ext_status);

  // exiting
  SharedMemoryManipulation (SHM_MANIP_CLIENT_EXIT, NULL, NULL);
}

static void
ShmFtruncateAndMmapFor (int shm_fd_num)
{
  switch (shm_fd_num)
    {
    case SHM_FD_CLIENTS_AMOUNT:
      // ftruncate doesn't needed
      if (shm_clients_amount == NULL)
        {
          if ((shm_clients_amount
               = (int *)mmap (NULL, sizeof (int), PROT_READ | PROT_WRITE,
                              MAP_SHARED, shm_fds[SHM_FD_CLIENTS_AMOUNT], 0))
              == (void *)-1)
            {
              err (EXIT_FAILURE, "mmap %d", __LINE__);
            }
        }
      break;

    case SHM_FD_MESSAGES_AMOUNT:
      // ftruncate doesn't needed
      if (shm_messages_amount == NULL)
        {
          if ((shm_messages_amount
               = (int *)mmap (NULL, sizeof (int), PROT_READ | PROT_WRITE,
                              MAP_SHARED, shm_fds[SHM_FD_MESSAGES_AMOUNT], 0))
              == (void *)-1)
            {
              err (EXIT_FAILURE, "mmap %d", __LINE__);
            }
        }
      break;

    case SHM_FD_CLIENTS_ID:
      if (ftruncate (shm_fds[SHM_FD_CLIENTS_ID],
                     (sizeof (char) * STRING_SIZE_MAX * (*shm_clients_amount))
                         + (sizeof (char *)))
          == -1)
        {
          err (EXIT_FAILURE, "ftruncate %d", __LINE__);
        }

      if ((*shm_clients_amount) != 0)
        {
          if ((shm_clients_id = (char *)mmap (
                   NULL,
                   (sizeof (char) * STRING_SIZE_MAX * (*shm_clients_amount)),
                   PROT_READ | PROT_WRITE, MAP_SHARED,
                   shm_fds[SHM_FD_CLIENTS_ID], 0))
              == (void *)-1)
            {
              err (EXIT_FAILURE, "mmap %d", __LINE__);
            }
        }
      break;

    case SHM_FD_MESSAGE_STRING:
      if (ftruncate (shm_fds[SHM_FD_MESSAGE_STRING],
                     (sizeof (char) * STRING_SIZE_MAX * (*shm_messages_amount))
                         + (sizeof (char *)))
          == -1)
        {
          err (EXIT_FAILURE, "ftruncate %d", __LINE__);
        }

      if ((*shm_messages_amount) != 0)
        {
          if ((shm_message_str = (char *)mmap (
                   NULL,
                   (sizeof (char) * STRING_SIZE_MAX * (*shm_messages_amount)),
                   PROT_READ | PROT_WRITE, MAP_SHARED,
                   shm_fds[SHM_FD_MESSAGE_STRING], 0))
              == (void *)-1)
            {
              err (EXIT_FAILURE, "mmap %d", __LINE__);
            }
        }
      break;

    case SHM_FD_MESSAGE_CLIENT_ID:
      if (ftruncate (shm_fds[SHM_FD_MESSAGE_CLIENT_ID],
                     (sizeof (char) * STRING_SIZE_MAX * (*shm_messages_amount))
                         + (sizeof (char *)))
          == -1)
        {
          err (EXIT_FAILURE, "ftruncate %d", __LINE__);
        }

      if ((*shm_messages_amount) != 0)
        {
          if ((shm_message_client_id = (char *)mmap (
                   NULL,
                   (sizeof (char) * STRING_SIZE_MAX * (*shm_messages_amount)),
                   PROT_READ | PROT_WRITE, MAP_SHARED,
                   shm_fds[SHM_FD_MESSAGE_CLIENT_ID], 0))
              == (void *)-1)
            {
              err (EXIT_FAILURE, "mmap %d", __LINE__);
            }
        }
      break;
    }
}

static void
ConnectAllSharedMemToProgramm ()
{
  for (int i = 0; i < SHM_FD_ELEMENTS_COUNT_; i++)
    {
      ShmFtruncateAndMmapFor (i);
    }
}

static void
GetSemaphoresAndAllSharedMemFds ()
{
  // open semaphore
  semaphore_main = sem_open (k_sem_names[SEMAPHORE_MAIN], O_RDWR);
  semaphore_exit = sem_open (k_sem_names[SEMAPHORE_EXIT], O_RDWR);

  // get all shared memory fds
  for (int i = 0; i < SHM_FD_ELEMENTS_COUNT_; i++)
    {
      shm_fds[i] = shm_open (k_shm_names[i], O_RDWR, 0666);
      if (shm_fds[i] == -1)
        err (EXIT_FAILURE, "shm_open %d", __LINE__);
    }
}

int
main ()
{
  GetSemaphoresAndAllSharedMemFds ();

  sem_wait (semaphore_main);
  ConnectAllSharedMemToProgramm ();
  sem_post (semaphore_main);

  InitGraphic ();

  ThreadsHandler ();

  return 0;
}