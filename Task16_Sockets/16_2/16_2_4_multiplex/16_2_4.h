#ifndef TCP_INET_H_
#define TCP_INET_H_

#define STR_SIZE_MAX 255

static const int k_server_main_port = 2000;

const char k_MSG_T_GIVE_ME_TIME = '1';
const char k_MSG_T_EXIT = '0';

const char *const k_socket_modes[2] = { "UDP", "TCP" };
typedef enum
{
  SOCK_MODE_UDP,
  SOCK_MODE_TCP
} SocketMode;

#endif // end TCP_INET_H_