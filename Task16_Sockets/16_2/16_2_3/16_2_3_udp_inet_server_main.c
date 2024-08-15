#include <arpa/inet.h>
#include <err.h>
#include <fcntl.h>
#include <mqueue.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "./16_2_3_udp_inet_header.h"

// init variables
const char *const k_exec_name = "16_2_3_udp_inet_server_sub";
const char *const k_exec_path_name = "./16_2_3_udp_inet_server_sub";
const int k_servers_pool_amount = 2;

mqd_t message_queue_of_server;
struct mq_attr mq_attr_struct = { 0, 10, STR_SIZE_MAX, 0, { 0 } };

pid_t *processes_pid;
int processes_amount;
typedef enum
{
  PROC_STAT__TRY_FIND = -3,
  PROC_STAT__POOL_CREATE,
  PROC_STAT__HAVENOT_FREE_PROC,
  PROC_STAT_FREE = 0,
  PROC_STAT_BUSY
} ProcessStatus;

int main_server_fd;
struct sockaddr_in main_server;
socklen_t sockaddr_len = sizeof (struct sockaddr);
//

static void
PrintErrorStrAndExit (char *err_str, int caller_line)
{
  err (EXIT_FAILURE, "[ %9s: %s: %d ]", k_server_types_str[SERV_T_MAIN_SERV],
       err_str, caller_line);
}

static void
CheckError (int err_int, char *err_str, int caller_line)
{
  if (err_int < 0)
    PrintErrorStrAndExit (err_str, caller_line);
}

static int
FindFreeProcOrCreateIt (int proc_stat_mode)
{
  int free_port_num = PROC_STAT__HAVENOT_FREE_PROC;
  pid_t fork_val = 0;

  if (proc_stat_mode == PROC_STAT__TRY_FIND)
    {
      for (int i = 0; i < processes_amount; i++)
        {
          if (processes_pid[i] == PROC_STAT_FREE)
            {
              processes_pid[i] = PROC_STAT_BUSY;
              free_port_num = i + 1;

              break;
            }
        }
    }

  if (free_port_num == PROC_STAT__HAVENOT_FREE_PROC)
    {
      processes_amount++;

      pid_t *temp_pid_ptr = (pid_t *)realloc (
          processes_pid, processes_amount * sizeof (pid_t));
      if (temp_pid_ptr == NULL)
        PrintErrorStrAndExit ("realloc", __LINE__);
      processes_pid = temp_pid_ptr;

      processes_pid[processes_amount - 1]
          = free_port_num == PROC_STAT__TRY_FIND ? PROC_STAT_BUSY
                                                 : PROC_STAT_FREE;

      free_port_num = processes_amount;

      // заготавливаем суб-сервер под нового клиента
      if ((fork_val = fork ()) == 0)
        {
          char port_str[20], main_server_fd_str[20];
          snprintf (port_str, sizeof (port_str) / sizeof (port_str[0]), "%d",
                    free_port_num);
          snprintf (main_server_fd_str,
                    sizeof (main_server_fd_str)
                        / sizeof (main_server_fd_str[0]),
                    "%d", main_server_fd);

          execl (k_exec_path_name, k_exec_name, port_str, main_server_fd_str,
                 (char *)NULL);
          PrintErrorStrAndExit ("execl", __LINE__);
        }
      else if (fork_val == -1)
        PrintErrorStrAndExit ("fork", __LINE__);
    }

  return free_port_num;
}

static void
RedirectClientsToSubServers ()
{
  char message_buf[STR_SIZE_MAX] = { 0 };
  char client_ip_str[INET_ADDRSTRLEN] = { 0 };

  struct sockaddr_in local_client;

  char mq_msg_send[STR_SIZE_MAX] = { 0 };

  CheckError (printf ("[ %9s: Waiting for clients to redirect ]\n",
                      k_server_types_str[SERV_T_MAIN_SERV]),
              "printf", __LINE__);

  while (1)
    {
      CheckError (recvfrom (main_server_fd, message_buf, sockaddr_len, 0,
                            (struct sockaddr *)&local_client, &sockaddr_len),
                  "recvfrom", __LINE__);

      CheckError (printf ("\n[ %9s: new client! ]\n",
                          k_server_types_str[SERV_T_MAIN_SERV]),
                  "printf", __LINE__);

      // получаем ip-адрес клиента в виде строки
      if (inet_ntop (AF_INET, &(local_client.sin_addr), client_ip_str,
                     INET_ADDRSTRLEN)
          == NULL)
        PrintErrorStrAndExit ("inet_ntop", __LINE__);

      // формируем и отправляем заявку вида:
      // { 'ip-адрес'\n'сообщение от клиента'\n }
      CheckError (snprintf (mq_msg_send, STR_SIZE_MAX, "%s\n%s\n",
                            client_ip_str, message_buf),
                  "snprintf", __LINE__);

      CheckError (
          mq_send (message_queue_of_server, mq_msg_send, STR_SIZE_MAX, 1),
          "mq_send", __LINE__);
    }
}

static void
InitMainServer ()
{
  // инициализируем настройки главного сервера с базовым портом
  main_server.sin_family = AF_INET;
  main_server.sin_port = htons (k_server_main_port);
  if (inet_pton (AF_INET, k_server_ip, &main_server.sin_addr) <= 0)
    PrintErrorStrAndExit ("inet_pton", __LINE__);

  // настраиваем соединение
  CheckError (main_server_fd = socket (main_server.sin_family, SOCK_DGRAM, 0),
              "socket", __LINE__);
  CheckError (
      bind (main_server_fd, (struct sockaddr *)&main_server, sockaddr_len),
      "bind", __LINE__);

  CheckError (printf ("-> HELP: Want to exit? Press Ctrl-C! <-\n\n"), "printf",
              __LINE__);
}

static void
SigExit ()
{
  exit (EXIT_SUCCESS);
}

static void
PrepareToExit ()
{

  CheckError (printf ("\n[ %9s: preparing to exit... ]\n",
                      k_server_types_str[SERV_T_MAIN_SERV]),
              "printf", __LINE__);

  CheckError (close (main_server_fd), "close", __LINE__);

  CheckError (printf ("\n[ %9s: close all sub servers... ]\n",
                      k_server_types_str[SERV_T_MAIN_SERV]),
              "printf", __LINE__);

  for (int i = 0; i < processes_amount; i++)
    {
      CheckError (kill (processes_pid[i], SIGINT), "kill", __LINE__);
      CheckError (wait (NULL), "wait", __LINE__);
    }

  mq_close (message_queue_of_server);
  mq_unlink (k_message_queue_name);

  free (processes_pid);

  CheckError (printf ("[ %9s: exit complete, bye-bye! ]\n\n",
                      k_server_types_str[SERV_T_MAIN_SERV]),
              "printf", __LINE__);
}

static void
MessageQueueOpen ()
{
  CheckError (message_queue_of_server
              = mq_open (k_message_queue_name, O_CREAT | O_WRONLY, 0644,
                         &mq_attr_struct),
              "mq_open", __LINE__);
}

int
main ()
{
  if (atexit (PrepareToExit) != 0)
    PrintErrorStrAndExit ("atexit", __LINE__);
  if (signal (SIGINT, SigExit) == SIG_ERR)
    PrintErrorStrAndExit ("signal", __LINE__);

  InitMainServer ();

  MessageQueueOpen ();

  for (int i = 0; i < k_servers_pool_amount; i++)
    {
      FindFreeProcOrCreateIt (PROC_STAT__POOL_CREATE);
    }

  RedirectClientsToSubServers ();

  return 0;
}