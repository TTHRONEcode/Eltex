#include <arpa/inet.h>
#include <err.h>
#include <linux/ip.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./16_5_raw_udp_ip.h"

static void CheckError(int err_n, char *err_str, int line) {
  if (err_n < 0) {
    err(EXIT_FAILURE, "%s: %d", err_str, line);
  }
}

int main() {
  struct udphdr udp_header;
  struct iphdr ip_header;
  struct sockaddr_in serv;

  int raw_sock_fd, socket_option = 1;

  char *msg_send = "[ RAW UDP CHECK ]", udp_msg[STR_SIZE_MAX] = {0},
       msg_recv[STR_SIZE_MAX] = {0};

  socklen_t struct_sock_lenght = sizeof(serv);

  // sock struct
  serv.sin_family = AF_INET;
  serv.sin_port = 0;
  serv.sin_addr.s_addr = INADDR_ANY;

  // ip struct
  ip_header.version = 4;
  ip_header.ihl = 5;
  ip_header.tos = 0;
  ip_header.tot_len = 0; // заполняется автоматически
  ip_header.id = 0; // заполняется автоматически, если 0
  ip_header.frag_off = 0;
  ip_header.ttl = 5;
  ip_header.protocol = IPPROTO_UDP;
  ip_header.check = 0; // заполняется автоматически
  ip_header.saddr = 0; // заполняется автоматически, если 0
  ip_header.daddr = htonl(INADDR_LOOPBACK);

  // udp struct
  udp_header.source = htons(PORT_CLIENT);
  udp_header.dest = htons(PORT_SERVER);
  udp_header.len = htons(sizeof(udp_header) + strlen(msg_send));
  udp_header.check = 0;

  // собираем заголовок из ip-заголовка + udp-заголовка + сообщения
  memcpy(udp_msg, &ip_header, sizeof(ip_header));
  memcpy(udp_msg + sizeof(ip_header), &udp_header, sizeof(udp_header));
  memcpy(udp_msg + sizeof(ip_header) + sizeof(udp_header), msg_send,
         strlen(msg_send));

  CheckError(raw_sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP), "socket",
             __LINE__);

  CheckError(setsockopt(raw_sock_fd, IPPROTO_IP, IP_HDRINCL, &socket_option,
                        sizeof(socket_option)),
             "setsockopt", __LINE__);

  CheckError(sendto(raw_sock_fd, (char *)udp_msg, STR_SIZE_MAX, 0,
                    (struct sockaddr *)&serv, struct_sock_lenght),
             "sendto", __LINE__);

  while (1) {
    CheckError(recvfrom(raw_sock_fd, (char *)msg_recv, STR_SIZE_MAX, 0,
                        (struct sockaddr *)&serv, &struct_sock_lenght),
               "recvfrom", __LINE__);

    // перепрыгиваем ip-заголовок, чтобы найти порт источника
    if (ntohs(*((unsigned short *)(msg_recv + sizeof(ip_header)))) ==
            PORT_SERVER &&
        // проверяем ip-адресс отправителя
        ntohl(*((unsigned int *)(msg_recv + sizeof(ip_header) -
                                 sizeof(ip_header.saddr) -
                                 sizeof(ip_header.daddr)))) == INADDR_LOOPBACK)
      break;
  }
  CheckError(printf("*Сообщение от сервера!: %s*\n", msg_recv + 28), "printf",
             __LINE__);

  CheckError(close(raw_sock_fd), "close", __LINE__);

  return 0;
}
