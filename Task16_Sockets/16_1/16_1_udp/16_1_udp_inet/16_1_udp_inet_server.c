#include <arpa/inet.h>
#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define STR_SIZE_MAX 255

static void CheckError(int err_n, char *err_str, int line) {
  if (err_n < 0) {
    err(EXIT_FAILURE, "%s: %d", err_str, line);
  }
}

int main() {
  struct sockaddr_in sockaddr_in_server, sockaddr_in_client;
  int server_sock_fd;
  char client_ip_buf[STR_SIZE_MAX] = {0};
  char msg_recv[STR_SIZE_MAX];

  socklen_t len = sizeof(struct sockaddr_in);

  sockaddr_in_server.sin_family = AF_INET;
  sockaddr_in_server.sin_addr.s_addr = INADDR_ANY;
  sockaddr_in_server.sin_port = htons(12345);

  CheckError(server_sock_fd =
                 socket(sockaddr_in_server.sin_family, SOCK_DGRAM, 0),
             "socket", __LINE__);

  CheckError(bind(server_sock_fd, (struct sockaddr *)&sockaddr_in_server, len),
             "bind", __LINE__);

  CheckError(recvfrom(server_sock_fd, msg_recv, STR_SIZE_MAX, 0,
                      (struct sockaddr *)&sockaddr_in_client, &len),
             "recvfrom", __LINE__);

  if (inet_ntop(AF_INET, &sockaddr_in_client.sin_addr.s_addr, client_ip_buf,
                STR_SIZE_MAX) == NULL)
    err(EXIT_FAILURE, "inet_ntop");

  CheckError(printf("CLIENT IP: %s\n"
                    "CLIENT PORT: %d\n"
                    "CLIENT MSG: %s\n",
                    client_ip_buf, ntohs(sockaddr_in_client.sin_port),
                    msg_recv),
             "printf", __LINE__);

  msg_recv[0] = ' ';

  CheckError(sendto(server_sock_fd, msg_recv, STR_SIZE_MAX, 0,
                    (struct sockaddr *)&sockaddr_in_client, len),
             "sendto", __LINE__);

  CheckError(close(server_sock_fd), "close", __LINE__);

  return 0;
}
