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
  char *msg_str = "Hello!";
  struct msgbuf msg_struct_snd, msg_struct_rcv;

  msg_struct_snd.mtype = 1;
  strncpy (msg_struct_snd.mtext, msg_str, strlen (msg_str));

  key_t key = ftok ("./13_1_V_server.c", 1);
  if (key == -1)
    err (EXIT_FAILURE, "ftok");

  int msg_id = msgget (key, IPC_CREAT | 6600);

  int msg_snd = msgsnd (msg_id, &msg_struct_snd, 8, 0);
  if (msg_snd == -1)
    err (EXIT_FAILURE, "msgsnd");

  ssize_t msg_rcv = msgrcv (msg_id, &msg_struct_rcv, 8, 2, 0);
  if (msg_rcv == -1)
    err (EXIT_FAILURE, "msg_rcv");
  else
    printf ("%s\n", msg_struct_rcv.mtext);

  int msg_ctl = msgctl (msg_id, IPC_RMID, 0);
  if (msg_ctl == -1)
    err (EXIT_FAILURE, "msg_ctl");

  return 0;
}