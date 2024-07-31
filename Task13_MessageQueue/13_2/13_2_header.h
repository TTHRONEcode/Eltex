#ifndef HEADER_H
#define HEADER_H

#define MSG_SIZE 255 * 2

#include <limits.h>
#include <mqueue.h>

char *name_server = "/SERVER";

struct mq_attr struct_mq_attr = {0, 10, MSG_SIZE, 0};

enum mq_types { MQ_T_NORMAL = 1, MQ_T_COPIES, MQ_T_INIT_CLIENT };

#endif // end HEADER_H