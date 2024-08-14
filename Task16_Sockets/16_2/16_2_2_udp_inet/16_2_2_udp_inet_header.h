#ifndef UDP_INET_H_
#define UDP_INET_H_

#define STR_SIZE_MAX 255

static const int k_server_main_port = 2000;

static const char k_MSG_T_GIVE_ME_TIME = '1';
static const char k_MSG_T_EXIT = '0';

static const char *const k_init_message = "INIT_ME";
static const char *const k_message_queue_name = "/16_2_2_UDP_INET_SERVER";
static const char *const k_template_i_am_free = "FREE";

static const char *const k_server_types_str[3]
    = { "MAIN SERV", "_SUB SERV", "CLIENT" };
typedef enum
{
  SERV_T_MAIN_SERV = 0,
  SERV_T_SUB_SERV,
  SERV_T_CLIENT
} ServerTypes;

#endif // end UDP_INET_H_