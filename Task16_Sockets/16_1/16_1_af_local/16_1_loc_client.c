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
  if (err_n == -1)
    {
      err (EXIT_FAILURE, "%s: %d", err_str, line);
    }
}

int
main ()
{
  struct sockaddr serv;
  int sock_fd;

  char *msg_send = "Hi!!";
  char msg_recv[5];

  int msg_size = 5;

  socklen_t len = sizeof (struct sockaddr);

  serv.sa_family = AF_LOCAL;
  strcpy (serv.sa_data, "/tmp/AF_LOCAL");

  CheckError (sock_fd = socket (AF_LOCAL, SOCK_STREAM, 0), "socket", __LINE__);

  CheckError (connect (sock_fd, &serv, len), "connect", __LINE__);

  CheckError (send (sock_fd, (char *)msg_send, msg_size, 0), "send", __LINE__);
  CheckError (recv (sock_fd, (char *)msg_recv, msg_size, 0), "recv", __LINE__);

  CheckError (printf ("%s\n", msg_recv), "printf", __LINE__);

  CheckError (close (sock_fd), "close", __LINE__);

  return 0;
}