#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <malloc.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "./13_2_graphic.h"
#include "./13_2_header.h"

enum
{
  MODE_LOGIN,
  MODE_MESSAGE
};

char **other_clients_msgs, **other_clients_msgs_names;
char *this_client_id, *msg_str_to_send, this_client_name[MSG_SIZE / 2],
    recieved_msg_str[MSG_SIZE / 2 + 1];
char entered_str[MSG_SIZE / 2] = { 0 };

int other_clients_msgs_count;
unsigned int ret_receive_prior;

mqd_t mqd_server, mqd_client;
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

  for (int i = 0; i < other_clients_msgs_count; i++)
    {
      free (other_clients_msgs[i]);
      free (other_clients_msgs_names[i]);
    }
  free (other_clients_msgs);
  free (other_clients_msgs_names);

  free (this_client_id);

  mq_close (mqd_client);

  mq_unlink (this_client_id);

  exit (EXIT_SUCCESS);
}

// 0 = for login
// 1 = for messages
void
EnterStdinString (int mode)
{

  int strlen_for = strlen (entered_str);
  for (int i = 0; i < strlen_for; i++)
    {
      entered_str[i] = 0;
    }

  int g_i = 0;
  while (g_i < MSG_SIZE / 2)
    {
      entered_str[g_i] = fgetc (stdin);

      if (entered_str[g_i] == '\n' || (mode == 0 && entered_str[g_i] == ' '))
        {
          if (mode == 0 && entered_str[g_i] == ' ')
            {
              ClearStdin ();
              entered_str[g_i] = 0;
            }
          break;
        }

      g_i++;
    }

  entered_str[g_i] = 0;
}

void *
ThreadToReceive (void *void_ptr)
{
  while (1)
    {
      ret_mq_receive = mq_receive (mqd_client, recieved_msg_str,
                                   MSG_SIZE + 3, &ret_receive_prior);
      if (dont_need_copy == true && ret_receive_prior == MQ_T_COPIES)
        {
          int strlen_n = strlen (recieved_msg_str);
          for (int i = 0; i < strlen_n; i++)
            {
              recieved_msg_str[i] = 0;
            }
          dont_need_copy = false;
          continue;
        }

      other_clients_msgs_count++;

      if (ret_mq_receive == -1)
        err (EXIT_FAILURE, "mq_receive");

      int n_start = 0;
      for (int i = 0; i < strlen (recieved_msg_str); i++)
        {
          if (recieved_msg_str[i] == '\n')
            {
              n_start = i;
              break;
            }
        }
      other_clients_msgs_names
          = realloc (other_clients_msgs_names,
                     other_clients_msgs_count * sizeof (char *));
      other_clients_msgs_names[other_clients_msgs_count - 1]
          = calloc (MSG_SIZE + 3, sizeof (char));

      other_clients_msgs = realloc (
          other_clients_msgs, other_clients_msgs_count * sizeof (char *));
      other_clients_msgs[other_clients_msgs_count - 1]
          = calloc (MSG_SIZE + 3, sizeof (char));

      strncpy (other_clients_msgs_names[other_clients_msgs_count - 1],
               recieved_msg_str, n_start);
      strncpy (other_clients_msgs[other_clients_msgs_count - 1],
               recieved_msg_str + n_start + 1,
               strlen (recieved_msg_str) - n_start - 1);

      printf ("%s > %s\n",
              other_clients_msgs_names[other_clients_msgs_count - 1],
              other_clients_msgs[other_clients_msgs_count - 1]);

      int strlen_n = strlen (recieved_msg_str);
      for (int i = 0; i < strlen_n; i++)
        {
          recieved_msg_str[i] = 0;
        }
    }

  return NULL;
}

void *
ThreadToSend (void *void_ptr)
{

  int ret_mq_send = mq_send (mqd_server, this_client_id,
                             strlen (this_client_id), MQ_T_INIT_CLIENT);
  if (ret_mq_send == -1)
    err (EXIT_FAILURE, "mq_send");

  while (1)
    {
      printf ("%s > ", this_client_name);

      EnterStdinString (MODE_MESSAGE);

      if (strcmp (entered_str, "exit") == 0)
        {
          printf ("*Выходим...\n");
          ClientExit ();
        }

      msg_str_to_send = calloc (strlen (this_client_name) + strlen ("\n")
                                    + strlen (entered_str) + 1,
                                sizeof (char));
      strncpy (msg_str_to_send, this_client_name, strlen (this_client_name));
      strcat (msg_str_to_send, "\n");
      strncat (msg_str_to_send, entered_str, strlen (entered_str));

      dont_need_copy = true;

      ret_mq_send = mq_send (mqd_server, msg_str_to_send,
                             strlen (msg_str_to_send), MQ_T_NORMAL);
      if (ret_mq_send == -1)
        err (EXIT_FAILURE, "mq_send");

      free (msg_str_to_send);
    }

  return NULL;
}

void
SetThisUniqClientId ()
{
  printf ("Введите логин: ");
  EnterStdinString (MODE_LOGIN);

  time_t time_not_str = time (NULL);
  char *time_to_str = asctime (gmtime (&time_not_str));

  this_client_id = calloc (strlen ("/") + strlen (entered_str) + strlen (" ")
                               + strlen (time_to_str) + 1,
                           sizeof (char));
  strncpy (this_client_id, "/", strlen ("/"));
  strncat (this_client_id, entered_str, strlen (entered_str));
  strcat (this_client_id, " ");
  strncat (this_client_id, time_to_str, strlen (time_to_str));

  strncpy (this_client_name, entered_str, strlen (entered_str));

  printf ("%s", this_client_id);
}

void
OpenMessageQueues ()
{
  mqd_client
      = mq_open (this_client_id, O_CREAT | O_RDONLY, S_IRWXU, &struct_mq_attr);
  if (mqd_client == -1)
    err (EXIT_FAILURE, "mq_open client");

  mqd_server = mq_open (name_server, O_WRONLY);
  if (mqd_server == -1)
    err (EXIT_FAILURE, "mq_open server");
}

void
StartAndWaitThread ()
{
  int pthread_ret
      = pthread_create (&threads[0], NULL, ThreadToSend, (void *)NULL);
  if (pthread_ret != 0)
    err (EXIT_FAILURE, "pthread_create");

  pthread_ret
      = pthread_create (&threads[1], NULL, ThreadToReceive, (void *)NULL);
  if (pthread_ret != 0)
    err (EXIT_FAILURE, "pthread_create");

  pthread_ret = pthread_join (threads[0], (void **)&thread_ext_status);
  if (pthread_ret != 0)
    err (EXIT_FAILURE, "pthread_join");

  pthread_ret = pthread_join (threads[1], (void **)&thread_ext_status);
  if (pthread_ret != 0)
    err (EXIT_FAILURE, "pthread_join");
}

void
InitClient ()
{
  // InitGraphic ();

  SetThisUniqClientId ();

  OpenMessageQueues ();

  StartAndWaitThread ();
}

int
main ()
{
  InitClient ();

  return 0;
}