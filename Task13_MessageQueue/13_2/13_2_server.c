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

char **client_msgs_login, **client_msgs;
int client_n, msgs_count;
mqd_t *temp;
char **temp_d;

mqd_t ret_mq_server;
mqd_t *ret_mq_client;

void
SignalHandler (int sig)
{
  if (sig == SIGINT)
    {
      printf ("\n Аварийное завершение \n");

      mq_close (ret_mq_server);

      for (int i = 0; i < client_n; i++)
        {
          mq_close (ret_mq_client[i]);
        }

      for (int i = 0; i < msgs_count; i++)
        {
          free (client_msgs[i]);
          free (client_msgs_login[i]);
        }

      free (client_msgs);
      free (client_msgs_login);

      free (temp);
      // free (temp_d);

      mq_unlink (name_server);

      exit (1);
    }
}

int
main ()
{
  signal (SIGINT, SignalHandler);

  char msg_str[MSG_SIZE + 2] = { 0 };
  unsigned int ret_receive_prior = 0;

  ssize_t ret_mq_receive;

  ret_mq_server
      = mq_open (name_server, O_CREAT | O_RDONLY, S_IRWXU, &struct_mq_attr);
  if (ret_mq_server == -1)
    err (EXIT_FAILURE, "mq_open server");

  int ret_mq_send = 0;

  //-----------------------------------------------------------------------

  while (1)
    {

      ret_mq_receive = mq_receive (ret_mq_server, msg_str, MSG_SIZE + 1,
                                   &ret_receive_prior);
      if (ret_mq_receive == -1)
        err (EXIT_FAILURE, "mq_receive");

      if (ret_receive_prior == GIVE_ME_YOUR_MSGS)
        {
          client_n++;

          temp = NULL;
          temp = realloc (ret_mq_client, sizeof (mqd_t) * client_n);
          if (temp == NULL)
            err (EXIT_FAILURE, "realloc 1");

          ret_mq_client = temp;

          ret_mq_client[client_n - 1] = mq_open (msg_str, O_CREAT | O_WRONLY,
                                                 S_IRWXU, &struct_mq_attr);
          if (ret_mq_client[client_n - 1] == -1)
            err (EXIT_FAILURE, "mq_open client");

          for (int i = 0; i < msgs_count; i++)
            {
              char *full_msg_str = calloc (MSG_SIZE + 3, sizeof (char *));

              strncpy (full_msg_str, client_msgs_login[i],
                       strlen (client_msgs_login[i]));
              strcat (full_msg_str, "\n");
              strncat (full_msg_str, client_msgs[i], strlen (client_msgs[i]));

              ret_mq_send = mq_send (ret_mq_client[client_n - 1], full_msg_str,
                                     strlen (full_msg_str), NORMAL);

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
      else if (ret_receive_prior == NORMAL)
        {
          int n_start = 0;

          msgs_count++;

          temp_d = NULL;
          temp_d = realloc (client_msgs, msgs_count * sizeof (char *));
          if (temp_d == NULL)
            err (EXIT_FAILURE, "realloc 2");
          client_msgs = temp_d;

          temp_d = NULL;
          temp_d = realloc (client_msgs_login, msgs_count * sizeof (char *));
          if (temp_d == NULL)
            err (EXIT_FAILURE, "realloc 2 l");
          client_msgs_login = temp_d;

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

          client_msgs[msgs_count - 1]
              = calloc (MSG_SIZE / 2 + 1, sizeof (char));
          client_msgs_login[msgs_count - 1]
              = calloc (MSG_SIZE / 2 + 1, sizeof (char));

          strncpy (client_msgs_login[msgs_count - 1], msg_str, n_start);

          strncpy (client_msgs[msgs_count - 1], msg_str + n_start + 1,
                   strlen (msg_str) - n_start - 1);

          for (int i = 0; i < client_n; i++)
            {
              ret_mq_send = mq_send (ret_mq_client[i], msg_str,
                                     strlen (msg_str), COPY_OF_MSG);
            }

          printf ("* %s > %s\n", client_msgs_login[msgs_count - 1],
                  client_msgs[msgs_count - 1]);
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