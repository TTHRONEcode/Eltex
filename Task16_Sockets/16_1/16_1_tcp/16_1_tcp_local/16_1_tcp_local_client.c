#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  struct sockaddr_un sockaddr_un_server;
  int server_sock_fd;

  char *msg_send = "Hi!!";
  char msg_recv[STR_SIZE_MAX];

  socklen_t len = sizeof(struct sockaddr);

  sockaddr_un_server.sun_family = AF_LOCAL;
  strcpy(sockaddr_un_server.sun_path, "/tmp/AF_LOCAL");

  CheckError(server_sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0), "socket",
             __LINE__);

  CheckError(
      connect(server_sock_fd, (struct sockaddr *)&sockaddr_un_server, len),
      "connect", __LINE__);

  CheckError(send(server_sock_fd, (char *)msg_send, STR_SIZE_MAX, 0), "send",
             __LINE__);
  CheckError(recv(server_sock_fd, (char *)msg_recv, STR_SIZE_MAX, 0), "recv",
             __LINE__);

  CheckError(
      getsockname(server_sock_fd, (struct sockaddr *)&sockaddr_un_server, &len),
      "getsockname", __LINE__);

  CheckError(printf("SERV PATH: %s\n"
                    "SERV MSG: %s\n",
                    sockaddr_un_server.sun_path, msg_recv),
             "printf", __LINE__);

  CheckError(close(server_sock_fd), "close", __LINE__);

  return 0;
}
