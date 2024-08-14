#include <err.h>
#include <mqueue.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "./16_2_2_udp_inet_header.h"

mqd_t message_queue_of_server;
int this_sub_server_port;

static void SubServerToClientInteraction (int __this_sub_server_port);

static void
PrintErrorStrAndExit (char *err_str, int caller_line)
{
  err (EXIT_FAILURE, "[ %9s [%d]: %s: %d ]", k_server_types_str[SERV_T_CLIENT],
       k_server_main_port + this_sub_server_port, err_str, caller_line);
}

static void
CheckError (int err_int, char *err_str, int caller_line)
{
  if (err_int < 0)
    PrintErrorStrAndExit (err_str, caller_line);
}

static void
SigExit ()
{
  exit (EXIT_SUCCESS);
}

static void
PrepareToExit ()
{
  CheckError (printf ("\n[ %9s [%d]: preparing to exit... ]\n",
                      k_server_types_str[SERV_T_SUB_SERV],
                      k_server_main_port + this_sub_server_port),
              "printf", __LINE__);

  mq_close (message_queue_of_server);
  mq_unlink (k_message_queue_name);

  CheckError (printf ("[ %9s [%d]: exit complete, bye-bye! ]\n\n",
                      k_server_types_str[SERV_T_SUB_SERV],
                      k_server_main_port + this_sub_server_port),
              "printf", __LINE__);
}

int
main (int argc, char *argv[])
{
  if (atexit (PrepareToExit) != 0)
    PrintErrorStrAndExit ("atexit", __LINE__);

  if (signal (SIGINT, SigExit) == SIG_ERR)
    PrintErrorStrAndExit ("signal", __LINE__);

  this_sub_server_port = atoi (argv[1]);
  SubServerToClientInteraction (this_sub_server_port);

  return 0;
}

static void
SubServerToClientInteraction (int this_sub_server_port)
{
  socklen_t sockaddr_len = sizeof (struct sockaddr);

  bool is_free = true;

  // переоткрываем очередь сообщений уже для записи
  char mq_msg_send[STR_SIZE_MAX] = { 0 };

  CheckError (message_queue_of_server
              = mq_open (k_message_queue_name, O_WRONLY),
              "mq_open", __LINE__);

  // инициализируем суб-сервер с новым портом
  char msg_recv[STR_SIZE_MAX] = { 0 };
  int sub_server_fd = 0;
  struct sockaddr_in sub_server = { 0 };

  sub_server.sin_family = AF_INET;
  sub_server.sin_port = htons (k_server_main_port + this_sub_server_port);
  sub_server.sin_addr.s_addr = INADDR_ANY;

  // настраиваем соединение
  CheckError (sub_server_fd = socket (sub_server.sin_family, SOCK_DGRAM, 0),
              "socket", __LINE__);
  CheckError (
      bind (sub_server_fd, (struct sockaddr *)&sub_server, sockaddr_len),
      "bind", __LINE__);

  CheckError (printf ("\n*Process №%d is getting started with port %d*\n",
                      this_sub_server_port,
                      k_server_main_port + this_sub_server_port),
              "printf", __LINE__);
  do
    {
      // ждём сообщения клиента и обрабатываем его
      CheckError (recvfrom (sub_server_fd, msg_recv, STR_SIZE_MAX, 0,
                            (struct sockaddr *)&sub_server, &sockaddr_len),
                  "recvfrom", __LINE__);

      if (is_free == true)
        {
          CheckError (
              printf ("*The process №%d is now busy*\n", this_sub_server_port),
              "printf", __LINE__);
          is_free = false;
        }

      if (msg_recv[0] == k_MSG_T_GIVE_ME_TIME)
        {
          time_t time_not_str = time (NULL);
          char *time_to_str = asctime (gmtime (&time_not_str));

          CheckError (sendto (sub_server_fd, time_to_str, sockaddr_len, 0,
                              (struct sockaddr *)&sub_server, sockaddr_len),
                      "recvfrom", __LINE__);

          CheckError (
              printf ("\n*№%d: The request has been successfully processed*\n",
                      this_sub_server_port),
              "printf", __LINE__);
        }
      else if (msg_recv[0] == k_MSG_T_EXIT)
        {
          printf ("\n*Client exit*\n");

          // уведомляем сервер о том, что этот процесс освободился
          CheckError (snprintf (mq_msg_send, STR_SIZE_MAX - 1, "%s:%d",
                                k_template_i_am_free, this_sub_server_port),
                      "snprintf", __LINE__);
          mq_msg_send[strlen (mq_msg_send)] = 0;

          CheckError (
              mq_send (message_queue_of_server, mq_msg_send, STR_SIZE_MAX, 1),
              "mq_send", __LINE__);
          //

          for (int i = 0; i < STR_SIZE_MAX; i++)
            {
              mq_msg_send[i] = 0;
            }

          printf ("*The process №%d is now free*\n"
                  "*Wait for another client...*\n",
                  this_sub_server_port);

          is_free = true;
        }
    }
  while (1);

  CheckError (printf ("*Process №%d is exiting*\n", this_sub_server_port),
              "printf", __LINE__);

  CheckError (close (sub_server_fd), "close", __LINE__);

  mq_close (message_queue_of_server);

  exit (EXIT_SUCCESS);
}
