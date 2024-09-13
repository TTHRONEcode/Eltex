#include <linux/if_link.h>
#include <net/if.h>

#include <arpa/inet.h>
#include <err.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./16_6.h"

// определение исходных данных устройств
#define THIS_IP_ADDR_SRC "192.168.0.13"
#define THIS_IP_ADDR_DEST "192.168.0.11"
#define THIS_INDEX_NAME "wls2"
#define THIS_MAC_ADDR_DEST                                                     \
  { 0xae, 0x39, 0x28, 0xf7, 0xd3, 0x7c }
#define THIS_MAC_ADDR_SRC                                                      \
  { 0x34, 0x68, 0x95, 0x76, 0x41, 0xfd }

static void CheckError(int err_n, char *err_str, int line) {
  if (err_n < 0) {
    err(EXIT_FAILURE, "%s: %d", err_str, line);
  }
}

__sum16 IpCheckSum(char *buf) {
  int csum = 0;
  __sum16 *ptr = (__sum16 *)buf;

  for (int i = 0; i < 10; i++) {
    csum += *ptr;
    ptr++;
  }
  int tmp = csum >> 16;
  csum = (csum & 0xFFFF) + tmp;
  csum = ~csum;

  return (__sum16)csum;
}

int main(void) {
  struct udphdr udp_header;
  struct iphdr ip_header;
  struct sockaddr_ll sockaddr_ll_server;
  struct ethhdr eth_header;

  int raw_sock_fd;

  char *msg_send = "[ RAW UDP CHECK ]", udp_msg[STR_SIZE_MAX] = {0},
       __attribute__((unused)) msg_recv[STR_SIZE_MAX] = {0};

  unsigned char mac_address_dest[ETH_ALEN] = THIS_MAC_ADDR_DEST;
  unsigned char mac_address_src[ETH_ALEN] = THIS_MAC_ADDR_SRC;

  socklen_t struct_sock_lenght = sizeof(sockaddr_ll_server);

  memset(&sockaddr_ll_server, 0, sizeof(sockaddr_ll_server));
  memset(&eth_header, 0, sizeof(eth_header));

  // sock struct
  sockaddr_ll_server.sll_family = AF_PACKET;
  sockaddr_ll_server.sll_ifindex = if_nametoindex(THIS_INDEX_NAME);
  sockaddr_ll_server.sll_halen = ETH_ALEN;
  memcpy(sockaddr_ll_server.sll_addr, mac_address_dest, ETH_ALEN);

  // ethernet struct
  memcpy(eth_header.h_dest, mac_address_dest, ETH_ALEN);
  memcpy(eth_header.h_source, mac_address_src, ETH_ALEN);
  eth_header.h_proto = htons(ETH_P_IP);

  // ip struct
  ip_header.version = 4;
  ip_header.ihl = 5;
  ip_header.tos = 0;
  ip_header.tot_len =
      htons(sizeof(ip_header) + sizeof(udp_header) + strlen(msg_send));
  ip_header.id = 0;
  ip_header.frag_off = 0;
  ip_header.ttl = 5;
  ip_header.protocol = IPPROTO_UDP;
  ip_header.check = 0;
  ip_header.saddr = inet_addr(THIS_IP_ADDR_SRC);
  ip_header.daddr = inet_addr(THIS_IP_ADDR_DEST);
  ip_header.check = IpCheckSum((char *)&ip_header);

  // udp struct
  udp_header.source = htons(PORT_CLIENT);
  udp_header.dest = htons(PORT_SERVER);
  udp_header.len = htons(sizeof(udp_header) + strlen(msg_send));
  udp_header.check = 0;

  // собираем заголовок из eth-заголовка ip-заголовка + udp-заголовка +
  // самого сообщения
  memcpy(udp_msg, &eth_header, sizeof(eth_header));
  memcpy(udp_msg + sizeof(eth_header), &ip_header, sizeof(ip_header));
  memcpy(udp_msg + sizeof(eth_header) + sizeof(ip_header), &udp_header,
         sizeof(udp_header));
  memcpy(udp_msg + sizeof(eth_header) + sizeof(ip_header) + sizeof(udp_header),
         msg_send, strlen(msg_send));

  // создаём сокет
  CheckError(raw_sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)),
             "socket", __LINE__);

  CheckError(sendto(raw_sock_fd, (char *)udp_msg, STR_SIZE_MAX, 0,
                    (struct sockaddr *)&sockaddr_ll_server, struct_sock_lenght),
             "sendto", __LINE__);

  while (1) {
    CheckError(recvfrom(raw_sock_fd, (char *)msg_recv, STR_SIZE_MAX, 0,
                        (struct sockaddr *)&sockaddr_ll_server,
                        &struct_sock_lenght),
               "recvfrom", __LINE__);

    // перепрыгиваем ip-заголовок, чтобы найти порт источника
    if (ntohs(*((unsigned short *)(msg_recv + sizeof(eth_header) +
                                   sizeof(ip_header)))) == PORT_SERVER) {
      // проверяем ip-адресс отправителя
      if (*((unsigned int *)(msg_recv + sizeof(eth_header) + sizeof(ip_header) -
                             sizeof(ip_header.saddr) -
                             sizeof(ip_header.daddr))) == ip_header.daddr)
        break;
    }
  }

  CheckError(printf("*Сообщение от сервера!: %s*\n",
                    msg_recv + sizeof(eth_header) + sizeof(ip_header) +
                        sizeof(udp_header)),
             "printf", __LINE__);

  CheckError(close(raw_sock_fd), "close", __LINE__);

  return 0;
}
