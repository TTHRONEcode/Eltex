#include <asm-generic/socket.h>
#include <err.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "16_3_broadcast.h"

static void CheckError(int err_n, char *err_str, int line) {
  if (err_n < 0) {
    err(EXIT_FAILURE, "%s: %d", err_str, line);
  }
}

int main() {
  struct sockaddr_in serv;
  int sock_fd;

  int opt = 1;
  char *msg_send = "[ BROADCAST ]";
  socklen_t len = sizeof(struct sockaddr);

  serv.sin_family = AF_INET;
  serv.sin_port = htons(12345);
  serv.sin_addr.s_addr = INADDR_BROADCAST;

  CheckError(sock_fd = socket(AF_INET, SOCK_DGRAM, 0), "socket", __LINE__);

  CheckError(
      setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, (int *)&opt, sizeof(int)),
      "setsockopt", __LINE__);

  CheckError(
      sendto(sock_fd, msg_send, STR_SIZE_MAX, 0, (struct sockaddr *)&serv, len),
      "sendto", __LINE__);

  CheckError(close(sock_fd), "close", __LINE__);

  return 0;
}
