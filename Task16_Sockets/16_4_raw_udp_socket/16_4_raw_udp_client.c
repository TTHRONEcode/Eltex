#include <err.h>
#include <linux/ip.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./16_4_raw_udp.h"

static void CheckError(int err_n, char *err_str, int line) {
  if (err_n < 0) {
    err(EXIT_FAILURE, "%s: %d", err_str, line);
  }
}

int main() {
  struct udphdr udp_header;

  struct sockaddr_in serv;
  int raw_sock_fd;

  char *msg_send = "[ RAW UDP CHECK ]";
  char udp_msg[STR_SIZE_MAX] = {0};
  char msg_recv[STR_SIZE_MAX] = {0};

  socklen_t len = sizeof(struct sockaddr);

  serv.sin_family = AF_INET;
  serv.sin_port = 0;
  serv.sin_addr.s_addr = INADDR_ANY;

  CheckError(raw_sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP), "socket",
             __LINE__);

  udp_header.source = htons(PORT_CLIENT);
  udp_header.dest = htons(PORT_SERVER);
  udp_header.len = htons(8 + strlen(msg_send));
  udp_header.check = 0;

  memcpy(udp_msg, &udp_header, 8);
  memcpy(udp_msg + 8, msg_send, strlen(msg_send));

  CheckError(sendto(raw_sock_fd, (char *)udp_msg, STR_SIZE_MAX, 0,
                    (struct sockaddr *)&serv, len),
             "send", __LINE__);

  while (1) {
    CheckError(recvfrom(raw_sock_fd, (char *)msg_recv, STR_SIZE_MAX, 0,
                        (struct sockaddr *)&serv, &len),
               "recv", __LINE__);

    //  msg destination
    if (ntohs(*((unsigned short *)(msg_recv + 20))) == PORT_SERVER)
      break;
  }

  CheckError(printf("*Сообщение от сервера!: %s*\n", msg_recv + 28), "printf",
             __LINE__);

  CheckError(close(raw_sock_fd), "close", __LINE__);

  return 0;
}
