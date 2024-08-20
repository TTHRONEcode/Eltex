#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./16_3_raw.h"

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
  struct sockaddr_in serv, client;
  int sock_fd;

  char msg_recv[STR_SIZE_MAX] = { 0 };
  char msg_send_change[3] = "___";

  socklen_t len = sizeof (struct sockaddr);

  serv.sin_family = AF_INET;
  serv.sin_port = htons (PORT_SERVER);
  serv.sin_addr.s_addr = INADDR_ANY;

  CheckError (sock_fd = socket (AF_INET, SOCK_DGRAM, 0), "socket", __LINE__);

  CheckError (bind (sock_fd, (struct sockaddr *)&serv, len), "bind", __LINE__);

  CheckError (recvfrom (sock_fd, msg_recv, STR_SIZE_MAX, 0,
                        (struct sockaddr *)&client, &len),
              "recvfrom", __LINE__);

  CheckError (printf ("*СООБЩЕНИЕ ОТ КЛИЕНТА: %s\n", msg_recv), "printf",
              __LINE__);

  strncpy (msg_recv + 2, msg_send_change, 3);

  CheckError (sendto (sock_fd, msg_recv, STR_SIZE_MAX, 0,
                      (struct sockaddr *)&client, len),
              "recvfrom", __LINE__);

  CheckError (close (sock_fd), "close", __LINE__);

  return 0;
}
