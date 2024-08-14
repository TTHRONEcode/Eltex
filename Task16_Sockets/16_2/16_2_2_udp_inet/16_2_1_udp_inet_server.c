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
#include <time.h>
#include <unistd.h>

#include "./16_2_1_udp_inet.h" //TODO rename to 2_2

const char *const k_template_i_am_free = "FREE";
const char *const k_message_queue_name = "/16_2_2_UDP_INET_SERVER";
mqd_t message_queue_of_server;
struct mq_attr mq_attr_struct = { 0, 10, STR_SIZE_MAX, 0, 0 };

const char *const k_servers_types_str[2] = { "MAIN SERV", "_SUB SERV" };
int this_server_type_num, this_server_port;

pid_t *processes_pid;
int processes_amount;
typedef enum
{
  PROC_STAT_HAVENOT_FREE_PROC = -1,
  PROC_STAT_FREE,
  PROC_STAT_BUSY
} ProcessStatus;

int main_server_fd;
struct sockaddr_in main_server;
socklen_t sockaddr_len = sizeof (struct sockaddr);

static void CheckError (int __err_int, char *__err_str, int __caller_line);

static void
PrintErrorStrAndExit (char *err_str, int caller_line)
{
  err (EXIT_FAILURE, "[ %9s [%d]: %s: %d ]",
       k_servers_types_str[this_server_type_num],
       k_server_main_port + this_server_port, err_str, caller_line);
}

static void
CheckError (int err_int, char *err_str, int caller_line)
{
  if (err_int < 0)
    PrintErrorStrAndExit (err_str, caller_line);
}

static void
SubServerToClientInteraction (int this_sub_server_port)
{
  bool is_free = true;

  // переоткрываем очередь сообщений уже для записи
  char mq_msg_send[STR_SIZE_MAX] = { 0 };

  // CheckError (mq_close (message_queue_of_server), "mq_close", __LINE__);
  // CheckError (message_queue_of_server
  //             = mq_open (k_message_queue_name, O_WRONLY),
  //             "mq_open", __LINE__);

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

static void
RedirectClientsToSubServers ()
{
  pid_t fork_val = 0;
  char message_buf[STR_SIZE_MAX] = { 0 };
  struct sockaddr_in local_client;
  int free_port_num = 0;

  // инициализируем и открываем очередь сообщений главного сервера для чтения
  char mq_msg_recv[STR_SIZE_MAX] = { 0 };
  ssize_t mq_read_bytes_amount = 0;
  ssize_t recvfrom_ret = 0;

  CheckError (message_queue_of_server
              = mq_open (k_message_queue_name, O_CREAT | O_RDWR | O_NONBLOCK,
                         0644, &mq_attr_struct),
              "mq_open", __LINE__);

  CheckError (printf ("[ %9s [%d]: Waiting for clients to redirect ]\n",
                      k_servers_types_str[this_server_type_num],
                      k_server_main_port + this_server_port),
              "printf", __LINE__);

  while (1)
    {
      // ждём нового клиента для последующей переадресации, параллельно
      // проверяя освободились ли процессы, чтобы их можно было снова занять
      while (1)
        {
          mq_read_bytes_amount = mq_receive (message_queue_of_server,
                                             mq_msg_recv, STR_SIZE_MAX, NULL);

          if (mq_msg_recv[0] != 0)
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

              mq_msg_recv[0] = 0;
            }

          errno = 0;

          recvfrom_ret = recvfrom (
              main_server_fd, message_buf, sockaddr_len, MSG_DONTWAIT,
              (struct sockaddr *)&local_client, &sockaddr_len);

          if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
              if (recvfrom_ret == -1)
                PrintErrorStrAndExit ("recvfrom", __LINE__);
              else
                {
                  CheckError (
                      printf ("\n[ %9s: new client! ]\n",
                              k_servers_types_str[this_server_type_num]),
                      "printf", __LINE__);
                  break; // клиент подключился!
                }
            }
          errno = 0;
        }
      errno = 0;

      free_port_num = PROC_STAT_HAVENOT_FREE_PROC;

      for (int i = 0; i < processes_amount; i++)
        {
          if (processes_pid[i] == PROC_STAT_FREE)
            {
              processes_pid[i] = PROC_STAT_BUSY;
              free_port_num = i + 1;

              break;
            }
        }

      if (free_port_num == PROC_STAT_HAVENOT_FREE_PROC)
        {
          processes_amount++;

          pid_t *temp_pid_ptr = (pid_t *)realloc (
              processes_pid, processes_amount * sizeof (pid_t));
          if (temp_pid_ptr == NULL)
            PrintErrorStrAndExit ("realloc", __LINE__);
          processes_pid = temp_pid_ptr;

          processes_pid[processes_amount - 1] = PROC_STAT_BUSY;

          free_port_num = processes_amount;

          // заготавливаем суб-сервер под нового клиента
          if ((fork_val = fork ()) == 0)
            {
              this_server_port = free_port_num;
              this_server_type_num = 1;
              SubServerToClientInteraction (free_port_num);
            }
          else if (fork_val == -1)
            PrintErrorStrAndExit ("fork", __LINE__);
        }

      // отправляем клиенту порт суб-сервера
      CheckError (sendto (main_server_fd, &(free_port_num), sizeof (int), 0,
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
                      k_servers_types_str[this_server_type_num],
                      k_server_main_port + this_server_port),
              "printf", __LINE__);

  CheckError (close (main_server_fd), "close", __LINE__);

  free (processes_pid);

  mq_close (message_queue_of_server);
  if (this_server_type_num == 0) // if it's MAIN SERV
    {
      mq_unlink (k_message_queue_name);
    }

  CheckError (printf ("[ %9s [%d]: exit complete, bye-bye! ]\n\n",
                      k_servers_types_str[this_server_type_num],
                      k_server_main_port + this_server_port),
              "printf", __LINE__);
}

int
main ()
{
  atexit (PrepareToExit);
  signal (SIGINT, SigExit);

  InitMainServer ();

  RedirectClientsToSubServers ();

  return 0;
}