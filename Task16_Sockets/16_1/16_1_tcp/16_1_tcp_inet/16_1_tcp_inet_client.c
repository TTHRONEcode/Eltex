#include <arpa/inet.h>
#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
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
  char ip_buf[STR_SIZE_MAX] = {0};
  char msg_recv[STR_SIZE_MAX] = {0};

  socklen_t len = sizeof(struct sockaddr_in);

  sockaddr_in_server.sin_family = AF_INET;
  sockaddr_in_server.sin_addr.s_addr = INADDR_ANY;
  sockaddr_in_server.sin_port = htons(12345);

  CheckError(server_sock_fd =
                 socket(sockaddr_in_server.sin_family, SOCK_STREAM, 0),
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

  if (inet_ntop(AF_INET, &sockaddr_in_server.sin_addr.s_addr, ip_buf,
                STR_SIZE_MAX) == NULL)
    err(EXIT_FAILURE, "inet_ntop");

  CheckError(printf("SERV IP: %s\n"
                    "SERV PORT: %d\n"
                    "SERV MSG: %s\n",
                    ip_buf, ntohs(sockaddr_in_server.sin_port), msg_recv),
             "printf", __LINE__);

  CheckError(close(server_sock_fd), "close", __LINE__);

  return 0;
}
