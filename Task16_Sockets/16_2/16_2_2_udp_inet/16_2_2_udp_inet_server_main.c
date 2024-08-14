#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "./16_2_2_udp_inet_header.h"

// init variables
const char *const k_exec_name = "16_2_2_udp_inet_server_sub";
const char *const k_exec_path_name = "./16_2_2_udp_inet_server_sub";
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
  err (EXIT_FAILURE, "[ %9s [%d]: %s: %d ]",
       k_server_types_str[SERV_T_MAIN_SERV], k_server_main_port, err_str,
       caller_line);
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
          char port_str[20];
          snprintf (port_str, sizeof (port_str) / sizeof (port_str[0]), "%d",
                    free_port_num);
          execl (k_exec_path_name, k_exec_name, port_str, (char *)NULL);
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
  int free_port_num = 0;
  char message_buf[STR_SIZE_MAX] = { 0 };
  struct sockaddr_in local_client;

  // инициализируем и открываем очередь сообщений главного сервера для чтения
  char mq_msg_recv[STR_SIZE_MAX] = { 0 };
  ssize_t mq_read_bytes_amount = 0;
  ssize_t recvfrom_ret = 0;

  CheckError (message_queue_of_server
              = mq_open (k_message_queue_name, O_CREAT | O_RDWR | O_NONBLOCK,
                         0644, &mq_attr_struct),
              "mq_open", __LINE__);

  CheckError (printf ("[ %9s [%d]: Waiting for clients to redirect ]\n",
                      k_server_types_str[SERV_T_MAIN_SERV],
                      k_server_main_port),
              "printf", __LINE__);

  while (1)
    {
      // ждём нового клиента для последующей переадресации, параллельно
      // проверяя освободились ли процессы, чтобы их можно было снова занять
      while (1)
        {
          mq_read_bytes_amount = mq_receive (message_queue_of_server,
                                             mq_msg_recv, STR_SIZE_MAX, NULL);

          // если пришло сообщение об освобождении другого процесса
          if (errno != EAGAIN)
            {
              if (mq_read_bytes_amount == -1)
                PrintErrorStrAndExit ("mq_receive", __LINE__);
              else
                {
                  if (strncmp (mq_msg_recv, k_template_i_am_free,
                               strlen (k_template_i_am_free))
                      == 0)
                    {
                      processes_pid
                          [atoi (
                               &mq_msg_recv[strlen (k_template_i_am_free) + 1])
                           - 1]
                          = PROC_STAT_FREE;
                    }
                }
            }
          errno = 0;

          recvfrom_ret = recvfrom (
              main_server_fd, message_buf, sockaddr_len, MSG_DONTWAIT,
              (struct sockaddr *)&local_client, &sockaddr_len);

          // если пришло сообщение от клиента
          if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
              if (recvfrom_ret == -1)
                PrintErrorStrAndExit ("recvfrom", __LINE__);
              else
                {
                  CheckError (printf ("\n[ %9s: new client! ]\n",
                                      k_server_types_str[SERV_T_MAIN_SERV]),
                              "printf", __LINE__);
                  break; // клиент подключился!
                }
            }
          errno = 0;
        }
      errno = 0;

      free_port_num = FindFreeProcOrCreateIt (PROC_STAT__TRY_FIND);

      // отправляем клиенту порт суб-сервера
      CheckError (sendto (main_server_fd, &free_port_num, sizeof (int), 0,
                          (struct sockaddr *)&local_client, sockaddr_len),
                  "recvfrom", __LINE__);
    }
}

static void
InitMainServer ()
{
  // инициализируем настройки главного сервера с базовым портом
  main_server.sin_family = AF_INET;
  main_server.sin_port = htons (k_server_main_port);
  main_server.sin_addr.s_addr = INADDR_ANY;

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

  CheckError (printf ("\n[ %9s [%d]: preparing to exit... ]\n",
                      k_server_types_str[SERV_T_MAIN_SERV],
                      k_server_main_port),
              "printf", __LINE__);

  CheckError (close (main_server_fd), "close", __LINE__);

  CheckError (printf ("\n[ %9s [%d]: close all sub servers... ]\n",
                      k_server_types_str[SERV_T_MAIN_SERV],
                      k_server_main_port),
              "printf", __LINE__);

  for (int i = 0; i < processes_amount; i++)
    {
      CheckError (kill (processes_pid[i], SIGINT), "kill", __LINE__);
      CheckError (wait (NULL), "wait", __LINE__);
    }

  mq_close (message_queue_of_server);
  mq_unlink (k_message_queue_name);

  free (processes_pid);

  CheckError (printf ("[ %9s [%d]: exit complete, bye-bye! ]\n\n",
                      k_server_types_str[SERV_T_MAIN_SERV],
                      k_server_main_port),
              "printf", __LINE__);
}

int
main ()
{
  if (atexit (PrepareToExit) != 0)
    PrintErrorStrAndExit ("atexit", __LINE__);
  if (signal (SIGINT, SigExit) == SIG_ERR)
    PrintErrorStrAndExit ("signal", __LINE__);

  InitMainServer ();

  for (int i = 0; i < k_servers_pool_amount; i++)
    {
      FindFreeProcOrCreateIt (PROC_STAT__POOL_CREATE);
    }

  RedirectClientsToSubServers ();

  return 0;
}