#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <malloc.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "./13_2_header.h"

char **other_clients_msgs_names, **other_clients_msgs;
char **clients_name;

int client_n, msgs_count;
mqd_t *temp;
char **temp_d;

mqd_t mqd_server;
mqd_t *mqd_client;

void
ServerExit ()
{
  mq_close (mqd_server);

  for (int i = 0; i < client_n; i++)
    {
      mq_close (mqd_client[i]);
    }

  for (int i = 0; i < msgs_count; i++)
    {
      free (other_clients_msgs[i]);
      free (other_clients_msgs_names[i]);
    }

  free (clients_name); // TODO clients_name[i] -> free
  free (other_clients_msgs);
  free (other_clients_msgs_names);

  free (temp);

  mq_unlink (name_server);

  exit (EXIT_SUCCESS);
}

void
SignalHandler (int sig)
{
  if (sig == SIGINT)
    {
      printf ("\n Аварийное завершение \n");

      ServerExit ();
    }
}

int
main ()
{
  signal (SIGINT, SignalHandler);

  char msg_str[MSG_SIZE + 2] = { 0 };
  unsigned int ret_receive_prior = 0;

  ssize_t ret_mq_receive;

  mqd_server
      = mq_open (name_server, O_CREAT | O_RDONLY, S_IRWXU, &struct_mq_attr);
  if (mqd_server == -1)
    err (EXIT_FAILURE, "mq_open server");

  int ret_mq_send = 0;

  //-----------------------------------------------------------------------

  while (1)
    {

      ret_mq_receive
          = mq_receive (mqd_server, msg_str, MSG_SIZE + 1, &ret_receive_prior);
      if (ret_mq_receive == -1)
        err (EXIT_FAILURE, "mq_receive");

      if (ret_receive_prior == MQ_T_INIT_CLIENT)
        {
          client_n++;

          temp_d = NULL;
          temp_d = realloc (clients_name, sizeof (char *) * client_n);
          if (temp_d == NULL)
            err (EXIT_FAILURE, "realloc 0");
          clients_name = temp_d;

          temp = NULL;
          temp = realloc (mqd_client, sizeof (mqd_t) * client_n);
          if (temp == NULL)
            err (EXIT_FAILURE, "realloc 1");

          mqd_client = temp;

          mqd_client[client_n - 1] = mq_open (msg_str, O_CREAT | O_WRONLY,
                                              S_IRWXU, &struct_mq_attr);
          if (mqd_client[client_n - 1] == -1)
            err (EXIT_FAILURE, "mq_open client");

          for (int i = 0; i < msgs_count; i++)
            {
              char *full_msg_str = calloc (MSG_SIZE + 3, sizeof (char *));

              strncpy (full_msg_str, other_clients_msgs_names[i],
                       strlen (other_clients_msgs_names[i]));
              strcat (full_msg_str, "\n");
              strncat (full_msg_str, other_clients_msgs[i],
                       strlen (other_clients_msgs[i]));

              ret_mq_send = mq_send (mqd_client[client_n - 1], full_msg_str,
                                     strlen (full_msg_str), MQ_T_NORMAL);

              free (full_msg_str);
            }

          printf ("*Пользователь ");
          for (int i = 1; i < MSG_SIZE; i++)
            {
              if (msg_str[i] != ' ')
                putchar (msg_str[i]);
              else
                break;
            }
          printf (" присоединился к чату!\n");
        }
      else if (ret_receive_prior == MQ_T_NORMAL)
        {
          int n_start = 0;

          msgs_count++;

          temp_d = NULL;
          temp_d = realloc (other_clients_msgs, msgs_count * sizeof (char *));
          if (temp_d == NULL)
            err (EXIT_FAILURE, "realloc 2");
          other_clients_msgs = temp_d;

          temp_d = NULL;
          temp_d = realloc (other_clients_msgs_names,
                            msgs_count * sizeof (char *));
          if (temp_d == NULL)
            err (EXIT_FAILURE, "realloc 2 l");
          other_clients_msgs_names = temp_d;

          //-----------------------------
          for (int i = 0; i < strlen (msg_str); i++)
            {
              if (msg_str[i] == '\n')
                {
                  n_start = i;
                  break;
                }
            }
          //-----------------------------

          other_clients_msgs[msgs_count - 1]
              = calloc (MSG_SIZE / 2 + 1, sizeof (char));
          other_clients_msgs_names[msgs_count - 1]
              = calloc (MSG_SIZE / 2 + 1, sizeof (char));

          strncpy (other_clients_msgs_names[msgs_count - 1], msg_str, n_start);

          strncpy (other_clients_msgs[msgs_count - 1], msg_str + n_start + 1,
                   strlen (msg_str) - n_start - 1);

          for (int i = 0; i < client_n; i++)
            {
              ret_mq_send = mq_send (mqd_client[i], msg_str, strlen (msg_str),
                                     MQ_T_COPIES);
            }

          printf ("* %s > %s\n", other_clients_msgs_names[msgs_count - 1],
                  other_clients_msgs[msgs_count - 1]);
        }

      int strlen_n = strlen (msg_str);
      for (int i = 0; i < strlen_n; i++)
        {
          msg_str[i] = 0;
        }
    }

  printf ("%s\n", msg_str);

  return 0;
}