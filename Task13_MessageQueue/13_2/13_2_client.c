#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <malloc.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "./13_2_header.h"

char **server_msgs, **client_msgs, **client_msgs_login;
char *this_name_client, *send_msg_str, login_str[MSG_SIZE / 2],
    msg_str[MSG_SIZE / 2 + 1];
char enter_str[MSG_SIZE / 2] = { 0 };

int msgs_from_server_n;
unsigned int ret_receive_prior;

mqd_t ret_mq_server, ret_mq_client;
ssize_t ret_mq_receive;

pthread_t threads[2];
int *thread_ext_status, dont_need_copy;

void
ClearStdin ()
{
  int c;
  while ((c = fgetc (stdin)) != EOF && c != '\n')
    ;
}

void
ClientExit ()
{
  int s;
  s = pthread_cancel (threads[0]);
  if (s != 0)
    err (EXIT_FAILURE, "pthread_candel, line:%d", __LINE__);
  s = pthread_cancel (threads[1]);
  if (s != 0)
    err (EXIT_FAILURE, "pthread_candel, line:%d", __LINE__);

  for (int i = 0; i < msgs_from_server_n; i++)
    {
      free (server_msgs[i]);
      free (client_msgs[i]);
      free (client_msgs_login[i]);
    }
  free (server_msgs);
  free (client_msgs);
  free (client_msgs_login);

  free (this_name_client);

  mq_close (ret_mq_client);

  mq_unlink (this_name_client);

  exit (EXIT_SUCCESS);

  // TODO ЗАКРЫТЬ ОЧЕРЕДЬ
}

// 0 = for login
// 1 = for messages
void
CharScanf (int mode)
{

  int strlen_for = strlen (enter_str);
  for (int i = 0; i < strlen_for; i++)
    {
      enter_str[i] = 0;
    }

  int g_i = 0;
  while (g_i < MSG_SIZE / 2)
    {
      enter_str[g_i] = fgetc (stdin);

      if (enter_str[g_i] == '\n' || (mode == 0 && enter_str[g_i] == ' '))
        {
          if (mode == 0 && enter_str[g_i] == ' ')
            {
              ClearStdin ();
              enter_str[g_i] = 0;
            }
          break;
        }

      g_i++;
    }

  enter_str[g_i] = 0;
}

void *
ThreadReceive (void *void_ptr)
{

  while (1)
    {

      ret_mq_receive = mq_receive (ret_mq_client, msg_str, MSG_SIZE + 3,
                                   &ret_receive_prior);
      if (ret_receive_prior == COPY_OF_MSG && dont_need_copy == 1)
        {
          int strlen_n = strlen (msg_str);
          for (int i = 0; i < strlen_n; i++)
            {
              msg_str[i] = 0;
            }
          dont_need_copy = 0;
          continue;
        }

      msgs_from_server_n++;
      server_msgs
          = realloc (server_msgs, msgs_from_server_n * sizeof (char *));
      server_msgs[msgs_from_server_n - 1]
          = calloc (MSG_SIZE + 3, sizeof (char));

      if (ret_mq_receive == -1)
        err (EXIT_FAILURE, "mq_receive");

      int n_start = 0;
      for (int i = 0; i < strlen (msg_str); i++)
        {
          if (msg_str[i] == '\n')
            {
              n_start = i;
              break;
            }
        }
      client_msgs_login
          = realloc (client_msgs_login, msgs_from_server_n * sizeof (char *));
      client_msgs_login[msgs_from_server_n - 1]
          = calloc (MSG_SIZE + 3, sizeof (char));
      client_msgs
          = realloc (client_msgs, msgs_from_server_n * sizeof (char *));
      client_msgs[msgs_from_server_n - 1]
          = calloc (MSG_SIZE + 3, sizeof (char));

      strncpy (client_msgs_login[msgs_from_server_n - 1], msg_str, n_start);
      strncpy (client_msgs[msgs_from_server_n - 1], msg_str + n_start + 1,
               strlen (msg_str) - n_start - 1);

      printf ("%s > %s\n", client_msgs_login[msgs_from_server_n - 1],
              client_msgs[msgs_from_server_n - 1]);

      int strlen_n = strlen (msg_str);
      for (int i = 0; i < strlen_n; i++)
        {
          msg_str[i] = 0;
        }
    }

  return NULL;
}

void *
ThreadSend (void *void_ptr)
{

  int ret_mq_send = mq_send (ret_mq_server, this_name_client,
                             strlen (this_name_client), GIVE_ME_YOUR_MSGS);
  if (ret_mq_send == -1)
    err (EXIT_FAILURE, "mq_send");

  pthread_create (&threads[1], NULL, ThreadReceive, (void *)NULL);

  while (1)
    {
      printf ("%s > ", login_str);

      CharScanf (1);

      if (strcmp (enter_str, "exit") == 0)
        {
          printf ("*Выходим...\n");
          ClientExit ();
        }

      send_msg_str = calloc (strlen (login_str) + strlen ("\n")
                                 + strlen (enter_str) + 1,
                             sizeof (char));
      strncpy (send_msg_str, login_str, strlen (login_str));
      strcat (send_msg_str, "\n");
      strncat (send_msg_str, enter_str, strlen (enter_str));

      ret_mq_send = mq_send (ret_mq_server, send_msg_str,
                             strlen (send_msg_str), NORMAL);
      dont_need_copy = 1;
      free (send_msg_str);
      if (ret_mq_send == -1)
        err (EXIT_FAILURE, "mq_send");
    }

  pthread_join (threads[1], (void **)&thread_ext_status);

  return NULL;
}

int
main ()
{
  time_t my_time = time (NULL);
  char *time_str = asctime (gmtime (&my_time));

  printf ("Введите логин: ");
  CharScanf (0);

  this_name_client
      = calloc (strlen (name_client) + strlen (enter_str) + strlen (" ")
                    + strlen (ctime (&my_time)) + 1,
                sizeof (char));
  strncpy (this_name_client, name_client, strlen (name_client));
  strncat (this_name_client, enter_str, strlen (enter_str));
  strcat (this_name_client, " ");
  strncat (this_name_client, ctime (&my_time), strlen (ctime (&my_time)));

  printf ("%s", this_name_client);

  strncpy (login_str, enter_str, strlen (enter_str));

  ret_mq_client = mq_open (this_name_client, O_CREAT | O_RDONLY, S_IRWXU,
                           &struct_mq_attr);
  if (ret_mq_client == -1)
    err (EXIT_FAILURE, "mq_open client");

  ret_mq_server = mq_open (name_server, O_WRONLY);
  if (ret_mq_server == -1)
    err (EXIT_FAILURE, "mq_open server");

  //-----------------------------------------------------------------------

  pthread_create (&threads[0], NULL, ThreadSend, (void *)NULL);
  pthread_join (threads[0], (void **)&thread_ext_status);

  free (this_name_client);

  return 0;
}