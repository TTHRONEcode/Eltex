#include <arpa/inet.h>
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

#include "./16_2_3_udp_inet_header.h"

mqd_t message_queue_of_server;
int this_sub_server_num;

static void SigExit ();
static void PrepareToExit ();
static void PrintErrorStrAndExit (char *__err_str, int __caller_line);
static void SubServerToClientInteraction (int __this_sub_server_port,
                                          int __sub_server_fd);

int
main (int argc __attribute__ ((unused)), char *argv[])
{
  if (atexit (PrepareToExit) != 0)
    PrintErrorStrAndExit ("atexit", __LINE__);

  if (signal (SIGINT, SigExit) == SIG_ERR)
    PrintErrorStrAndExit ("signal", __LINE__);

  this_sub_server_num = atoi (argv[1]);
  SubServerToClientInteraction (this_sub_server_num, atoi (argv[2]));

  return 0;
}

static void
PrintErrorStrAndExit (char *err_str, int caller_line)
{
  err (EXIT_FAILURE, "[ %9s [%d]: %s: %d ]",
       k_server_types_str[SERV_T_SUB_SERV], this_sub_server_num, err_str,
       caller_line);
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
                      this_sub_server_num),
              "printf", __LINE__);

  mq_close (message_queue_of_server);

  CheckError (printf ("[ %9s [%d]: exit complete, bye-bye! ]\n\n",
                      k_server_types_str[SERV_T_SUB_SERV],
                      this_sub_server_num),
              "printf", __LINE__);
}

static void
SubServerToClientInteraction (int sub_server_num, int sub_server_fd)
{
  socklen_t sockaddr_len = sizeof (struct sockaddr);
  char recv_ip[STR_SIZE_MAX] = { 0 };
  int j;

  // переоткрываем очередь сообщений уже для записи
  char mq_msg_recv[STR_SIZE_MAX] = { 0 };

  CheckError (message_queue_of_server
              = mq_open (k_message_queue_name, O_RDONLY),
              "mq_open", __LINE__);

  // инициализируем суб-сервер
  char client_request = 0;
  struct sockaddr_in sub_server = { 0 };

  sub_server.sin_family = AF_INET;
  sub_server.sin_port = htons (k_server_main_port);

  CheckError (printf ("\n*Process %d is getting started*\n", sub_server_num),
              "printf", __LINE__);
  do
    {
      CheckError (mq_receive (message_queue_of_server, mq_msg_recv,
                              STR_SIZE_MAX, NULL),
                  "mq_receive", __LINE__);

      // находим ip-адресс и сообщение в полученном запросе
      for (j = 0; j < STR_SIZE_MAX; j++)
        {
          if (mq_msg_recv[j] == '\n')
            break;
        }

      strncpy (recv_ip, mq_msg_recv, j);

      if (inet_pton (AF_INET, recv_ip, &sub_server.sin_addr) <= 0)
        PrintErrorStrAndExit ("inet_pton", __LINE__);

      client_request = *(mq_msg_recv + j + 1);
      //

      if (client_request == k_MSG_T_GIVE_ME_TIME)
        {
          time_t time_not_str = time (NULL);
          char *time_to_str = asctime (gmtime (&time_not_str));

          CheckError (sendto (sub_server_fd, time_to_str, sockaddr_len, 0,
                              (struct sockaddr *)&sub_server, sockaddr_len),
                      "recvfrom", __LINE__);

          CheckError (printf ("\n* %s [%d]: The request has been successfully "
                              "processed *\n",
                              k_server_types_str[SERV_T_SUB_SERV],
                              sub_server_num),
                      "printf", __LINE__);
        }
      else if (client_request == k_MSG_T_EXIT)
        {
          printf ("\n* %s [%d]: client exited *\n",
                  k_server_types_str[SERV_T_SUB_SERV], sub_server_num);

          for (int i = 0; i < STR_SIZE_MAX; i++)
            {
              mq_msg_recv[i] = 0;
            }

          printf ("* %s [%d]: now free *\n"
                  "* %s [%d]: wait for another client... *\n",
                  k_server_types_str[SERV_T_SUB_SERV], sub_server_num,
                  k_server_types_str[SERV_T_SUB_SERV], sub_server_num);
        }
      else if (client_request == k_MSG_T_INIT_ME)
        {
          CheckError (sendto (sub_server_fd, "_", sockaddr_len, 0,
                              (struct sockaddr *)&sub_server, sockaddr_len),
                      "recvfrom", __LINE__);

          CheckError (printf ("\n* %s [%d]: The client has been successfully "
                              "init *\n",
                              k_server_types_str[SERV_T_SUB_SERV],
                              sub_server_num),
                      "printf", __LINE__);
        }
    }
  while (1);

  CheckError (printf ("* %s [%d]: exiting... *\n",
                      k_server_types_str[SERV_T_SUB_SERV], sub_server_num),
              "printf", __LINE__);

  exit (EXIT_SUCCESS);
}
