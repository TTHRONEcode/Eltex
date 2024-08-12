#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define N 7

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
  struct sockaddr_in serv;
  int sock_fd;

  char *msg_send = "Hi!!";
  char msg_recv[N];

  int msg_size = N;

  socklen_t len = sizeof (struct sockaddr);

  serv.sin_family = AF_INET;
  serv.sin_port = htons (12345);
  serv.sin_addr.s_addr = INADDR_ANY;

  CheckError (sock_fd = socket (AF_INET, SOCK_DGRAM, 0), "socket", __LINE__);

  CheckError (connect (sock_fd, (struct sockaddr *)&serv, len), "connect",
              __LINE__);

  CheckError (send (sock_fd, (char *)msg_send, msg_size, 0), "send", __LINE__);

  CheckError (recv (sock_fd, (char *)msg_recv, msg_size, 0), "recv", __LINE__);

  CheckError (printf ("%s\n", msg_recv), "printf", __LINE__);

  CheckError (close (sock_fd), "close", __LINE__);

  return 0;
}
