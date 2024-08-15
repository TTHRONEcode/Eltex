#include <arpa/inet.h>
#include <ctype.h>
#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./16_2_3_udp_inet_header.h"

const char *const subnet = "127.0.0.";
char ip_addr_str[INET_ADDRSTRLEN];

static void
PrintErrorStrAndExit (char *err_str, int caller_line)
{
  err (EXIT_FAILURE, "[ %9s [%d]: %s: %d ]", k_server_types_str[SERV_T_CLIENT],
       k_server_main_port, err_str, caller_line);
}

static void
CheckError (int err_int, char *err_str, int caller_line)
{
  if (err_int < 0)
    PrintErrorStrAndExit (err_str, caller_line);
}

static void
InteractWithSubServerViaSockFd (int sock_fd)
{
  char msg_recv[STR_SIZE_MAX] = { 0 };
  char msg_send[STR_SIZE_MAX] = { 0 };

  CheckError (
      printf ("*[%s]: Пробуем подключиться к серверу...*\n", ip_addr_str),
      "printf", __LINE__);

  msg_send[0] = k_MSG_T_INIT_ME;
  CheckError (send (sock_fd, (char *)msg_send, STR_SIZE_MAX, 0), "send",
              __LINE__);
  CheckError (recv (sock_fd, (char *)msg_recv, STR_SIZE_MAX, 0), "recv",
              __LINE__);

  CheckError (printf ("*[%s]: Подключение успешно!*\n", ip_addr_str), "printf",
              __LINE__);

  do
    {
      printf ("\n*Введите номер действия:*\n"
              "*%c. Запросить время/дату сервера*\n"
              "*%c. Выйти*\n"
              "> ",
              k_MSG_T_GIVE_ME_TIME, k_MSG_T_EXIT);

      msg_send[0] = getchar ();

      int c;
      while ((c = fgetc (stdin)) != EOF && c != '\n')
        ;

      if (msg_send[0] == k_MSG_T_GIVE_ME_TIME)
        {
          CheckError (send (sock_fd, (char *)msg_send, STR_SIZE_MAX, 0),
                      "send", __LINE__);

          CheckError (recv (sock_fd, (char *)msg_recv, STR_SIZE_MAX, 0),
                      "recv", __LINE__);

          CheckError (printf ("%s\n", msg_recv), "printf", __LINE__);
        }
      else
        {
          CheckError (send (sock_fd, (char *)msg_send, STR_SIZE_MAX, 0),
                      "send", __LINE__);

          break;
        }

      msg_send[0] = 0;
    }
  while (1);
}

static void
InputNodeForIp (char *const restrict node_str)
{
  int ret_getchar = 0;

  while (1)
    {
      for (int i = 0; i < 3; i++)
        {
          node_str[i] = 0;
        }

      printf ("*Введите узел ip-адреса(2-255): 127.0.0.");

      for (int i = 0; i < 3; i++)
        {
          ret_getchar = getchar ();
          if (ret_getchar == '\n')
            {
              break;
            }
          else
            {
              if (isdigit (ret_getchar) != 0)
                {
                  node_str[i] = (char)ret_getchar;
                }
              else
                i--;
            }
        }
      if (atoi (node_str) <= 1 || atoi (node_str) >= 256)
        {
          CheckError (printf ("\n*!Неправильно введён ip-адрес!*\n"), "printf",
                      __LINE__);
        }
      else
        break;
    }
}

static int
GetSubServerSockFd ()
{
  struct sockaddr_in local_addr, server_addr;
  char node_str[3] = { 0 }; // 3 -> 3x -> 127.0.0.xxx
  int sock_fd;

  socklen_t len = sizeof (struct sockaddr);

  // инициализируем себя со своим ip-адресом
  InputNodeForIp (node_str);

  strncat (ip_addr_str, subnet, strlen (subnet) + 1);
  strncat (ip_addr_str, node_str, strlen (node_str) + 1);

  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons (k_server_main_port);
  if (inet_pton (AF_INET, ip_addr_str, &local_addr.sin_addr) <= 0)
    PrintErrorStrAndExit ("inet_pton", __LINE__);

  // создаём сокет
  CheckError (sock_fd = socket (local_addr.sin_family, SOCK_DGRAM, 0),
              "socket", __LINE__);
  CheckError (bind (sock_fd, (struct sockaddr *)&local_addr, len), "bind",
              __LINE__);

  // определяем адресс сервера и присоединяемся к нему
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons (k_server_main_port);
  if (inet_pton (AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    PrintErrorStrAndExit ("inet_pton", __LINE__);

  CheckError (connect (sock_fd, (struct sockaddr *)&server_addr, len),
              "connect", __LINE__);

  return sock_fd;
}

int
main ()
{
  // start server interaction after connect to a sub server
  InteractWithSubServerViaSockFd (GetSubServerSockFd ());

  return 0;
}