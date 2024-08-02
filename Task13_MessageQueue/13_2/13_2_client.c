#include <curses.h>
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
char **other_id;
int clients_count;

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
ClientExit ()
{

  int ret_mq_send = mq_send (mqd_server, this_client_id,
                             strlen (this_client_id), MQ_T_CLIENT_EXIT);
  if (ret_mq_send == -1)
    err (EXIT_FAILURE, "mq_send");

  for (int i = 0; i < other_clients_msgs_count; i++)
    {
      free (other_clients_msgs[i]);
      free (other_clients_msgs_names[i]);
    }
  for (int i = 0; i < clients_count; i++)
    {
      free (other_id[i]);
    }

  free (other_id);
  free (other_clients_msgs);
  free (other_clients_msgs_names);

  mq_close (mqd_client);
  mq_close (mqd_server);

  mq_unlink (this_client_id);
  free (this_client_id);

  DelWins ();

  endwin ();

  exit (EXIT_SUCCESS);

  int pthread_ex_status;
  pthread_ex_status = pthread_cancel (threads[1]);
  if (pthread_ex_status != 0)
    err (EXIT_FAILURE, "pthread_cancel, line:%d", __LINE__);
  pthread_ex_status = pthread_cancel (threads[0]);
  if (pthread_ex_status != 0)
    err (EXIT_FAILURE, "pthread_cancel, line:%d", __LINE__);
}

void
MsgsAdd (char *local_msg_str, int is_this)
{

  int n_start = 0;
  for (int i = 0; i < strlen (local_msg_str); i++)
    {
      if (local_msg_str[i] == '\n')
        {
          n_start = i;
          break;
        }
    }

  other_clients_msgs_count++;

  other_clients_msgs_names = realloc (
      other_clients_msgs_names, other_clients_msgs_count * sizeof (char *));
  other_clients_msgs_names[other_clients_msgs_count - 1]
      = calloc (MSG_SIZE + 3, sizeof (char));

  other_clients_msgs = realloc (other_clients_msgs,
                                other_clients_msgs_count * sizeof (char *));
  other_clients_msgs[other_clients_msgs_count - 1]
      = calloc (MSG_SIZE + 3, sizeof (char));

  if (is_this == 0)
    {

      strncpy (other_clients_msgs_names[other_clients_msgs_count - 1],
               local_msg_str, n_start);
      strncpy (other_clients_msgs[other_clients_msgs_count - 1],
               local_msg_str + n_start + 1,
               strlen (local_msg_str) - n_start - 1);
    }
  else
    {
      strncpy (other_clients_msgs_names[other_clients_msgs_count - 1], "", 2);
      strncpy (other_clients_msgs[other_clients_msgs_count - 1],
               local_msg_str + n_start + 1,
               strlen (local_msg_str) - n_start - 1);
    }

  RenderInputMsgs (is_this);
}

static void
CleanReceiveMsg ()
{
  int strlen_n = strlen (recieved_msg_str);
  for (int i = 0; i < strlen_n; i++)
    {
      recieved_msg_str[i] = 0;
    }
}

// 0 = for login
// 1 = for messages
void *
ThreadToReceive (void *void_ptr)
{
  while (1)
    {
      ret_mq_receive = mq_receive (mqd_client, recieved_msg_str, MSG_SIZE + 3,
                                   &ret_receive_prior);
      if (ret_mq_receive == -1)
        err (EXIT_FAILURE, "mq_receive");

      if (ret_receive_prior == MQ_T_COPIES)
        {
          if (dont_need_copy == true)
            {
              CleanReceiveMsg ();
              dont_need_copy = false;
              continue;
            }
        }
      else if (ret_receive_prior == MQ_T_CLIENT_ENTER)
        {

          clients_count++;

          other_id = realloc (other_id, clients_count * sizeof (char *));
          other_id[clients_count - 1] = calloc (MSG_SIZE + 3, sizeof (char));

          strncpy (other_id[clients_count - 1], recieved_msg_str,
                   strlen (recieved_msg_str));
          RenderIds ();
          CleanReceiveMsg ();
          continue;
        }
      else if (ret_receive_prior == MQ_T_CLIENT_EXIT)
        {

          for (int i = 0; i < clients_count; i++)
            {
              if (strcmp (other_id[i], recieved_msg_str) == 0)
                {
                  strncpy (other_id[i], other_id[clients_count - 1],
                           strlen (other_id[i]));
                  free (other_id[clients_count - 1]);

                  clients_count--;

                  RenderIds ();
                  CleanReceiveMsg ();
                  break;
                }
            }

          continue;
        }
      if (ret_receive_prior == MQ_T_SERVER_EXIT)
        {
          ClientExit ();
        }

      MsgsAdd (recieved_msg_str, 0);

      CleanReceiveMsg ();
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
      EnterStdinString (MODE_MESSAGE, entered_str);

      char exit_str[5] = "exit";
      if (strcmp (entered_str, exit_str) == 0)
        {
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

      MsgsAdd (msg_str_to_send, 1);

      free (msg_str_to_send);
    }

  return NULL;
}

void
SetThisUniqClientId ()
{
  PutLoginText ();
  EnterStdinString (MODE_LOGIN, entered_str);

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

  PrintThisId (this_client_id);
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
    {
      SignalExit (0);
      err (EXIT_FAILURE, "mq_open server");
    }
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
  InitGraphic ();

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