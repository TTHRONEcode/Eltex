#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define STR_SIZE_MAX 255

const char *const lock_sock_path = "/tmp/LOCAL_UDP_SOCKET";
const char *const lock_client_path = "/tmp/LOCAL_UDP_SOCKET_CLIENT";

static void CheckError(int err_n, char *err_str, int line) {
  if (err_n == -1) {
    err(EXIT_FAILURE, "%s: %d", err_str, line);
  }
}

int main() {
  struct sockaddr_un sockaddr_un_server, sockaddr_un_client;
  int client_sock_fd;

  char *msg_send = "Hi!!";
  char msg_recv[STR_SIZE_MAX];

  socklen_t len = sizeof(struct sockaddr_un);

  sockaddr_un_server.sun_family = AF_LOCAL;
  strncpy(sockaddr_un_server.sun_path, lock_sock_path,
          strlen(lock_sock_path) + 1);

  sockaddr_un_client.sun_family = AF_LOCAL;
  strncpy(sockaddr_un_client.sun_path, lock_client_path,
          strlen(lock_client_path) + 1);

  CheckError(client_sock_fd =
                 socket(sockaddr_un_client.sun_family, SOCK_DGRAM, 0),
             "socket", __LINE__);
  CheckError(bind(client_sock_fd, (struct sockaddr *)&sockaddr_un_client, len),
             "bind", __LINE__);

  CheckError(
      connect(client_sock_fd, (struct sockaddr *)&sockaddr_un_server, len),
      "connect", __LINE__);

  CheckError(send(client_sock_fd, (char *)msg_send, STR_SIZE_MAX, 0), "send",
             __LINE__);

  CheckError(recv(client_sock_fd, (char *)msg_recv, STR_SIZE_MAX, 0), "recv",
             __LINE__);

  CheckError(printf("SERV PATH: %s\n"
                    "SERV MSG: %s\n",
                    sockaddr_un_server.sun_path, msg_recv),
             "printf", __LINE__);

  CheckError(close(client_sock_fd), "close", __LINE__);
  CheckError(unlink(lock_client_path), "unlink", __LINE__);

  return 0;
}
