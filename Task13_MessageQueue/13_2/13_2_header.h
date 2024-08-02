#ifndef INC_13_2_HEADER_H
#define INC_13_2_HEADER_H

#define MSG_SIZE 255 * 2

#include <limits.h>
#include <mqueue.h>

const char *const name_server = "/SERVER";

const struct mq_attr struct_mq_attr = {0, 10, MSG_SIZE, 0};

enum mq_types { MQ_T_NORMAL = 1, MQ_T_COPIES, MQ_T_INIT_CLIENT, MQ_T_CLIENT_ENTER, MQ_T_CLIENT_EXIT, MQ_T_SERVER_EXIT };

#endif // INC_13_2_HEADER_H
