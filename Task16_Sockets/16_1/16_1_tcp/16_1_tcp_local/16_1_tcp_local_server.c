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
  if (err_n < 0) {
    err(EXIT_FAILURE, "%s: %d", err_str, line);
  }
}

int main() {
  struct sockaddr_un sockaddr_un_server, sockaddr_un_client;
  int server_sock_fd, client_sock_fd;

  char msg_recv_send[STR_SIZE_MAX] = {0};

  socklen_t len = sizeof(struct sockaddr_un);

  sockaddr_un_server.sun_family = AF_LOCAL;
  strncpy(sockaddr_un_server.sun_path, "/tmp/AF_LOCAL",
          sizeof(sockaddr_un_server.sun_path) - 1);

  CheckError(server_sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0), "socket",
             __LINE__);

  CheckError(bind(server_sock_fd, (struct sockaddr *)&sockaddr_un_server, len),
             "bind", __LINE__);

  CheckError(listen(server_sock_fd, 5), "listen", __LINE__);

  CheckError(client_sock_fd = accept(
                 server_sock_fd, (struct sockaddr *)&sockaddr_un_client, &len),
             "accept", __LINE__);

  CheckError(recv(client_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0),
             "recv", __LINE__);

  CheckError(
      getpeername(client_sock_fd, (struct sockaddr *)&sockaddr_un_client, &len),
      "getsockname", __LINE__);

  CheckError(printf("CLIENT PATH: %s\n"
                    "CLIENT MSG: %s\n",
                    sockaddr_un_client.sun_path, msg_recv_send),
             "printf", __LINE__);

  msg_recv_send[0] = '_';

  CheckError(send(client_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0),
             "send", __LINE__);

  CheckError(close(client_sock_fd), "close", __LINE__);
  CheckError(close(server_sock_fd), "close", __LINE__);

  CheckError(unlink(sockaddr_un_server.sun_path), "unlink", __LINE__);

  return 0;
}
