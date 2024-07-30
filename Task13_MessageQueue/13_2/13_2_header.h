#ifndef HEADER_H
#define HEADER_H

#define MSG_SIZE 255 * 2

#include <limits.h>
#include <mqueue.h>

char *name_server = "/SERVER", *name_client = "/";

struct mq_attr struct_mq_attr = {0, 10, MSG_SIZE, 0};

enum { NORMAL = 1, LAST_MSG, COPY_OF_MSG, GIVE_ME_YOUR_MSGS = 5 };

#endif // end HEADER_H