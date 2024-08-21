#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./16_2_2_udp_inet_header.h"

int this_client_port;

static void
PrintErrorStrAndExit (char *err_str, int caller_line)
{
  err (EXIT_FAILURE, "[ %9s [%d]: %s: %d ]", k_server_types_str[SERV_T_CLIENT],
       k_server_main_port + this_client_port, err_str, caller_line);
}

static void
CheckError (int err_int, char *err_str, int caller_line)
{
  if (err_int < 0)
    PrintErrorStrAndExit (err_str, caller_line);
}

static void
ServerInteraction (int sock_fd)
{
  char msg_recv[STR_SIZE_MAX] = { 0 };
  char typed_char[STR_SIZE_MAX] = { 0 };

  do
    {
      printf ("\n*Введите номер действия:*\n"
              "*%c. Запросить время/дату сервера*\n"
              "*%c. Выйти*\n"
              "> ",
              k_MSG_T_GIVE_ME_TIME, k_MSG_T_EXIT);

      typed_char[0] = getchar ();

      int c;
      while ((c = fgetc (stdin)) != EOF && c != '\n')
        ;

      if (typed_char[0] == k_MSG_T_GIVE_ME_TIME)
        {
          CheckError (send (sock_fd, (char *)typed_char, STR_SIZE_MAX, 0),
                      "send", __LINE__);

          CheckError (recv (sock_fd, (char *)msg_recv, STR_SIZE_MAX, 0),
                      "recv", __LINE__);

          CheckError (printf ("%s\n", msg_recv), "printf", __LINE__);
        }
      else
        {
          CheckError (send (sock_fd, (char *)typed_char, STR_SIZE_MAX, 0),
                      "send", __LINE__);

          break;
        }

      typed_char[0] = 0;
    }
  while (1);

  CheckError (close (sock_fd), "close", __LINE__);
}

static int
InitConnectionToSubServer ()
{
  struct sockaddr_in serv;
  int sock_fd;

  socklen_t len = sizeof (struct sockaddr);

  serv.sin_family = AF_INET;
  serv.sin_port = htons (k_server_main_port);
  serv.sin_addr.s_addr = INADDR_ANY;

  // connect to main server and request a port offset
  CheckError (sock_fd = socket (serv.sin_family, SOCK_DGRAM, 0), "socket",
              __LINE__);
  CheckError (connect (sock_fd, (struct sockaddr *)&serv, len), "connect",
              __LINE__);
  CheckError (send (sock_fd, k_init_message, STR_SIZE_MAX, 0), "send",
              __LINE__);

  // receive the port offset to connect to a sub server
  CheckError (recv (sock_fd, &this_client_port, sizeof (int), 0), "recv",
              __LINE__);

  // reconnect to a sub server via new port
  serv.sin_port = htons (k_server_main_port + this_client_port);
  CheckError (connect (sock_fd, (struct sockaddr *)&serv, len), "connect",
              __LINE__);

  return sock_fd;
}

int
main ()
{
  // start server interaction after connect to a sub server
  ServerInteraction (InitConnectionToSubServer ());

  return 0;
}