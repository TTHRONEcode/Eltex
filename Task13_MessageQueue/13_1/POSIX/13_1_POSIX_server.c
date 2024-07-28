#include <err.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

int
main ()
{
  char *name_str_1 = "/NOT_SYSTEM_V_1", *name_str_2 = "/NOT_SYSTEM_V_2";
  char msg_str[33] = { 0 };

  struct mq_attr struct_mq_attr = { 0, 10, 33, 0 };

  mqd_t ret_mq_open
      = mq_open (name_str_1, O_CREAT | O_WRONLY, 6600, &struct_mq_attr);

  int ret_mq_send = mq_send (ret_mq_open, "Hello!", 7, 1);
  if (ret_mq_send == -1)
    err (EXIT_FAILURE, "mq_send");
  mq_close (ret_mq_open);

  ret_mq_open
      = mq_open (name_str_2, O_CREAT | O_RDONLY, 6600, &struct_mq_attr);

  ssize_t ret_mq_receive = mq_receive (ret_mq_open, msg_str, 33, NULL);
  if (ret_mq_receive == -1)
    err (EXIT_FAILURE, "mq_receive");

  printf ("%s\n", msg_str);

  mq_close (ret_mq_open);

  mq_unlink (name_str_1);
  mq_unlink (name_str_2);

  return 0;
}