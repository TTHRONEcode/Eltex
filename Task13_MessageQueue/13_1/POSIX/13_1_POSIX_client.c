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
  char msg_str[50] = { 0 };

  mqd_t ret_mq_open = mq_open (name_str_1, O_RDONLY);

  ssize_t ret_mq_receive = mq_receive (ret_mq_open, msg_str, 50, NULL);
  if (ret_mq_receive == -1)
    {
      mq_close (ret_mq_open);

      mq_unlink (name_str_1);
      err (EXIT_FAILURE, "mq_receive");
    }

  printf ("%s", msg_str);
  mq_close (ret_mq_open);

  ret_mq_open = mq_open (name_str_2, O_WRONLY);

  int ret_mq_send = mq_send (ret_mq_open, "Hi!\n", 3, 1);
  if (ret_mq_send == -1)
    err (EXIT_FAILURE, "mq_send");

  mq_close (ret_mq_open);

  return 0;
}