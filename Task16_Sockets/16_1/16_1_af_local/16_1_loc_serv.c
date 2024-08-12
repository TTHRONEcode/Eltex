#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static void
CheckError (int err_n, char *err_str, int line)
{
  if (err_n < 0)
    {
      err (EXIT_FAILURE, "%s: %d", err_str, line);
    }
}

int
main ()
{
  struct sockaddr serv, client;
  int sock_fd, clients_fd;

  char message[5] = { 0 };
  int msg_size = 5;

  socklen_t len = sizeof (struct sockaddr);

  serv.sa_family = AF_LOCAL;
  strcpy (serv.sa_data, "/tmp/AF_LOCAL");

  CheckError (sock_fd = socket (AF_LOCAL, SOCK_STREAM, 0), "socket", __LINE__);

  CheckError (bind (sock_fd, &serv, len), "bind", __LINE__);

  CheckError (listen (sock_fd, 5), "listen", __LINE__);

  CheckError (clients_fd = accept (sock_fd, &client, &len), "accept",
              __LINE__);

  CheckError (recv (clients_fd, (char *)message, msg_size, 0), "recv",
              __LINE__);

  CheckError (printf ("%s\n", message), "printf", __LINE__);

  message[0] = ' ';

  CheckError (send (clients_fd, (char *)message, msg_size, 0), "send",
              __LINE__);
  CheckError (recv (clients_fd, (char *)message, msg_size, 0), "recv",
              __LINE__);

  CheckError (close (clients_fd), "close", __LINE__);
  CheckError (close (sock_fd), "close", __LINE__);

  CheckError (unlink (serv.sa_data), "unlink", __LINE__);

  return 0;
}