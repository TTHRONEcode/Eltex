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
  int server_sock_fd, clients_sock_fd;

  char msg_recv_send[STR_SIZE_MAX] = {0};
  char client_ip_buf[STR_SIZE_MAX] = {0};

  socklen_t len = sizeof(struct sockaddr_in);

  sockaddr_in_server.sin_family = AF_INET;
  sockaddr_in_server.sin_addr.s_addr = INADDR_ANY;
  sockaddr_in_server.sin_port = htons(12345);

  CheckError(server_sock_fd =
                 socket(sockaddr_in_server.sin_family, SOCK_STREAM, 0),
             "socket", __LINE__);

  CheckError(bind(server_sock_fd, (struct sockaddr *)&sockaddr_in_server, len),
             "bind", __LINE__);

  CheckError(listen(server_sock_fd, 5), "listen", __LINE__);

  CheckError(clients_sock_fd = accept(
                 server_sock_fd, (struct sockaddr *)&sockaddr_in_client, &len),
             "accept", __LINE__);

  CheckError(recv(clients_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0),
             "recv", __LINE__);

  CheckError(getpeername(clients_sock_fd,
                         (struct sockaddr *)&sockaddr_in_client, &len),
             "getpeername", __LINE__);

  if (inet_ntop(AF_INET, &sockaddr_in_client.sin_addr.s_addr, client_ip_buf,
                STR_SIZE_MAX) == NULL)
    err(EXIT_FAILURE, "inet_ntop");

  CheckError(printf("CLIENT IP: %s\n"
                    "CLIENT PORT: %d\n"
                    "CLIENT MSG: %s\n",
                    client_ip_buf, ntohs(sockaddr_in_client.sin_port),
                    msg_recv_send),
             "printf", __LINE__);

  msg_recv_send[0] = ' ';

  CheckError(send(clients_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0),
             "send", __LINE__);
  CheckError(recv(clients_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0),
             "recv", __LINE__);

  CheckError(close(clients_sock_fd), "close", __LINE__);
  CheckError(close(server_sock_fd), "close", __LINE__);

  return 0;
}
