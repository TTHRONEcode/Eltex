#include <arpa/inet.h>
#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "16_3_multicast.h"

static void CheckError(int err_n, char *err_str, int line) {
  if (err_n == -1) {
    err(EXIT_FAILURE, "%s: %d", err_str, line);
  }
}

int main() {
  struct sockaddr_in serv;
  struct ip_mreqn mreqn;

  int sock_fd;

  char msg_recv[STR_SIZE_MAX];

  socklen_t len = sizeof(struct sockaddr);

  serv.sin_family = AF_INET;
  serv.sin_port = htons(SERVER_PORT);
  serv.sin_addr.s_addr = INADDR_ANY;

  memset(&mreqn, 0, sizeof(mreqn));
  mreqn.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP_ADDR);
  mreqn.imr_address.s_addr = INADDR_ANY;
  mreqn.imr_ifindex = 0;

  CheckError(sock_fd = socket(AF_INET, SOCK_DGRAM, 0), "socket", __LINE__);

  setsockopt(sock_fd, IPPROTO_TP, IP_ADD_MEMBERSHIP, (int *)&mreqn,
             sizeof(mreqn));

  CheckError(bind(sock_fd, (struct sockaddr *)&serv, len), "bind", __LINE__);

  while (1) {
    CheckError(recvfrom(sock_fd, msg_recv, STR_SIZE_MAX, 0,
                        (struct sockaddr *)&serv, &len),
               "recvfrom", __LINE__);

    CheckError(printf("%s\n", msg_recv), "printf", __LINE__);
  }

  CheckError(close(sock_fd), "close", __LINE__);

  return 0;
}
