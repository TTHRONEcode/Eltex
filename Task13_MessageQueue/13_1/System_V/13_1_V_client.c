#include <complex.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct msgbuf
{
  long mtype;
  char mtext[8];
};

int
main ()
{
  char *msg_str = "Hi!";
  struct msgbuf msg_struct_snd, msg_struct_rcv;

  msg_struct_snd.mtype = 2;
  strncpy (msg_struct_snd.mtext, msg_str, strlen (msg_str));

  key_t key = ftok ("./13_1_V_server.c", 1);
  if (key == -1)
    err (EXIT_FAILURE, "ftok");

  int msg_id = msgget (key, 0);

  ssize_t msg_rcv = msgrcv (msg_id, &msg_struct_rcv, 8, 1, 0);
  if (msg_rcv == -1)
    err (EXIT_FAILURE, "msg_rcv");
  else
    printf ("%s\n", msg_struct_rcv.mtext);

  int msg_snd = msgsnd (msg_id, &msg_struct_snd, 8, 0);
  if (msg_snd == -1)
    err (EXIT_FAILURE, "msgsnd");

  return 0;
}