#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define STR_SIZE_MAX 255

const char *const lock_sock_path = "/tmp/LOCAL_UDP_SOCKET";

static void CheckError(int err_n, char *err_str, int line) {
  if (err_n < 0) {
    err(EXIT_FAILURE, "%s: %d", err_str, line);
  }
}

int main() {
  struct sockaddr_un sockaddr_un_serv, sockaddr_un_client;
  int server_sock_fd;

  char msg_recv[STR_SIZE_MAX];

  socklen_t len = sizeof(struct sockaddr_un);

  sockaddr_un_serv.sun_family = AF_LOCAL;
  strncpy(sockaddr_un_serv.sun_path, lock_sock_path,
          strlen(lock_sock_path) + 1);

  CheckError(server_sock_fd =
                 socket(sockaddr_un_serv.sun_family, SOCK_DGRAM, 0),
             "socket", __LINE__);

  CheckError(bind(server_sock_fd, (struct sockaddr *)&sockaddr_un_serv, len),
             "bind", __LINE__);

  CheckError(recvfrom(server_sock_fd, msg_recv, STR_SIZE_MAX, 0,
                      (struct sockaddr *)&sockaddr_un_client, &len),
             "recvfrom", __LINE__);

  CheckError(printf("CLIENT PATH: %s\n"
                    "CLIENT MSG: %s\n",
                    sockaddr_un_client.sun_path, msg_recv),
             "printf", __LINE__);

  msg_recv[0] = '_';

  CheckError(sendto(server_sock_fd, msg_recv, STR_SIZE_MAX, 0,
                    (struct sockaddr *)&sockaddr_un_client, len),
             "sendto", __LINE__);

  CheckError(close(server_sock_fd), "close", __LINE__);
  CheckError(unlink(lock_sock_path), "unlink", __LINE__);

  return 0;
}
