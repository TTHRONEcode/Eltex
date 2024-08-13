#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./16_2_1_tcp_inet.h"

static void
CheckError (int err_int, char *err_str, int caller_line)
{
  if (err_int < 0)
    {
      err (EXIT_FAILURE, "%s: %d", err_str, caller_line);
    }
}

static void
TalkToServer (int sock_fd)
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
}

int
main ()
{
  struct sockaddr_in serv;
  int sock_fd;

  char *msg_send = "Hi!!";
  char msg_recv[STR_SIZE_MAX];

  socklen_t len = sizeof (struct sockaddr);

  serv.sin_family = AF_INET;
  serv.sin_port = htons (12345);
  serv.sin_addr.s_addr = INADDR_ANY;

  CheckError (sock_fd = socket (AF_INET, SOCK_STREAM, 0), "socket", __LINE__);
  CheckError (connect (sock_fd, (struct sockaddr *)&serv, len), "connect",
              __LINE__);

  TalkToServer (sock_fd);

  CheckError (close (sock_fd), "close", __LINE__);

  return 0;
}