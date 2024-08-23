#include <arpa/inet.h>
#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define STR_SIZE_MAX 255

static void CheckError(int err_n, char *err_str, int line) {
  if (err_n == -1) {
    err(EXIT_FAILURE, "%s: %d", err_str, line);
  }
}

int main() {
  struct sockaddr_in sockaddr_in_server;
  int server_sock_fd;

  char *msg_send = "Hi!!";
  char server_ip_buf[STR_SIZE_MAX] = {0};
  char msg_recv[STR_SIZE_MAX];

  socklen_t len = sizeof(struct sockaddr_in);

  sockaddr_in_server.sin_family = AF_INET;
  sockaddr_in_server.sin_port = htons(12345);
  sockaddr_in_server.sin_addr.s_addr = INADDR_ANY;

  CheckError(server_sock_fd =
                 socket(sockaddr_in_server.sin_family, SOCK_DGRAM, 0),
             "socket", __LINE__);

  CheckError(
      connect(server_sock_fd, (struct sockaddr *)&sockaddr_in_server, len),
      "connect", __LINE__);

  CheckError(send(server_sock_fd, (char *)msg_send, STR_SIZE_MAX, 0), "send",
             __LINE__);

  CheckError(recv(server_sock_fd, (char *)msg_recv, STR_SIZE_MAX, 0), "recv",
             __LINE__);

  CheckError(
      getpeername(server_sock_fd, (struct sockaddr *)&sockaddr_in_server, &len),
      "getpeername", __LINE__);

  if (inet_ntop(AF_INET, &sockaddr_in_server.sin_addr.s_addr, server_ip_buf,
                STR_SIZE_MAX) == NULL)
    err(EXIT_FAILURE, "inet_ntop");

  CheckError(printf("SERV IP: %s\n"
                    "SERV PORT: %d\n"
                    "SERV MSG: %s\n",
                    server_ip_buf, ntohs(sockaddr_in_server.sin_port),
                    msg_recv),
             "printf", __LINE__);

  CheckError(close(server_sock_fd), "close", __LINE__);

  return 0;
}
