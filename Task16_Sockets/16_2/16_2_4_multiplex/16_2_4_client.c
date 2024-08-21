#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./16_2_4.h"

static void
ClearStdIn ()
{
  int c;
  while ((c = getchar ()) != EOF && c != '\n')
    ;
}

static void
CheckError (int err_int, char *err_str, int caller_line)
{
  if (err_int < 0)
    {
      err (EXIT_FAILURE, "%s: %d", err_str, caller_line);
    }
}

static void
InteractWithServerViaSockFd (int sock_fd)
{
  char msg_recv[255] = { 0 };
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

int
main ()
{
  struct sockaddr_in serv;
  int sock_fd, typed_char = 0, this_client_port_offset = 0;

  char *msg_send = "_init_placeholder_";

  socklen_t len = sizeof (struct sockaddr);

  serv.sin_family = AF_INET;
  serv.sin_port = htons (k_server_main_port);
  serv.sin_addr.s_addr = INADDR_ANY;

  printf ("*Введите тип подключения:\n"
          "%d. %s\n"
          "%d. %s\n"
          "> ",
          SOCK_MODE_UDP, k_socket_modes[SOCK_MODE_UDP], SOCK_MODE_TCP,
          k_socket_modes[SOCK_MODE_TCP]);

  typed_char = getchar ();
  ClearStdIn ();

  if (atoi ((char *)&typed_char) == SOCK_MODE_UDP)
    {
      CheckError (sock_fd = socket (serv.sin_family, SOCK_DGRAM, 0), "socket",
                  __LINE__);
      CheckError (connect (sock_fd, (struct sockaddr *)&serv, len), "connect",
                  __LINE__);

      CheckError (send (sock_fd, (char *)msg_send, STR_SIZE_MAX, 0), "send",
                  __LINE__);

      // receive the port offset to connect to a sub server
      CheckError (recv (sock_fd, &this_client_port_offset, sizeof (int), 0),
                  "recv", __LINE__);

      // reconnect to a sub server via new port
      serv.sin_port = htons (k_server_main_port + this_client_port_offset);
      CheckError (connect (sock_fd, (struct sockaddr *)&serv, len), "connect",
                  __LINE__);
    }
  else if (atoi ((char *)&typed_char) == SOCK_MODE_TCP)
    {
      CheckError (sock_fd = socket (AF_INET, SOCK_STREAM, 0), "socket",
                  __LINE__);
      CheckError (connect (sock_fd, (struct sockaddr *)&serv, len), "connect",
                  __LINE__);
    }

  InteractWithServerViaSockFd (sock_fd);

  return 0;
}