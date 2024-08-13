#include <err.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "./16_2_1_tcp_inet.h"

struct sockaddr_in serv, client;
int sock_fd;

socklen_t len = sizeof (struct sockaddr);

static void CheckError (int __err_int, char *__err_str, int __caller_line);

static void
PrintErrorAndExit (char *err_str, int caller_line)
{
  err (EXIT_FAILURE, "%s: %d", err_str, caller_line);
}

static void
CheckError (int err_int, char *err_str, int caller_line)
{
  if (err_int < 0)
    PrintErrorAndExit (err_str, caller_line);
}

void
ProcSocketClient (int loc_sock_fd, int loc_clients_fd, int this_proc_num)
{
  char msg_recv[STR_SIZE_MAX] = { 0 };

  CheckError (printf ("\n*Process №%d is getting started with fd %d*\n",
                      this_proc_num, loc_clients_fd),
              "printf", __LINE__);
  do
    {
      CheckError (recv (loc_clients_fd, (char *)msg_recv, STR_SIZE_MAX, 0),
                  "recv", __LINE__);

      if (msg_recv[0] == k_MSG_T_GIVE_ME_TIME)
        {
          time_t time_not_str = time (NULL);
          char *time_to_str = asctime (gmtime (&time_not_str));

          CheckError (
              send (loc_clients_fd, (char *)time_to_str, STR_SIZE_MAX, 0),
              "send", __LINE__);
        }
      else
        {
          break;
        }

      CheckError (
          printf ("*%d*\n*The request has been successfully processed*\n",
                  this_proc_num),
          "printf", __LINE__);
    }
  while (1);

  CheckError (printf ("*Process №%d is exiting*\n", this_proc_num), "printf",
              __LINE__);

  CheckError (close (loc_clients_fd), "close", __LINE__);
  CheckError (close (loc_sock_fd), "close", __LINE__);

  exit (EXIT_SUCCESS);
}

static void
SocketHandler ()
{
  pid_t fork_val = 0;
  char message_buf[STR_SIZE_MAX] = { 0 };
  int proc_count = 0, clients_fd = 0;

  do
    {
      CheckError (printf ("[ MAIN PROC: Wait for client ]\n"), "printf",
                  __LINE__);
      CheckError (clients_fd
                  = accept (sock_fd, (struct sockaddr *)&client, &len),
                  "accept", __LINE__);
      CheckError (printf ("[ MAIN PROC: Client connected with fd = %d ]\n\n",
                          clients_fd),
                  "printf", __LINE__);

      proc_count++;

      if ((fork_val = fork ()) == 0)
        ProcSocketClient (sock_fd, clients_fd, proc_count);
      else if (fork_val == -1)
        PrintErrorAndExit ("fork", __LINE__);
    }
  while (1);
}

static void
SocketInitProcClose ()
{
  CheckError (printf ("-> HELP: Want to exit? Press Ctrl-C! <-\n\n"), "printf",
              __LINE__);

  serv.sin_family = AF_INET;
  serv.sin_port = htons (12345);
  serv.sin_addr.s_addr = INADDR_ANY;

  CheckError (sock_fd = socket (AF_INET, SOCK_STREAM, 0), "socket", __LINE__);

  CheckError (bind (sock_fd, (struct sockaddr *)&serv, len), "bind", __LINE__);

  CheckError (listen (sock_fd, 2), "listen", __LINE__);

  SocketHandler ();

  CheckError (close (sock_fd), "close", __LINE__);
}

static void
SigExit ()
{
  exit (EXIT_SUCCESS);
}

static void
PrepareToExit ()
{
  CheckError (printf ("\n[ MAIN PROC: preparing to exit... ]\n"), "printf",
              __LINE__);

  CheckError (printf ("[ MAIN PROC: closing sock_fd... ]\n"), "printf",
              __LINE__);
  CheckError (close (sock_fd), "close", __LINE__);
  CheckError (printf ("[ MAIN PROC: bye-bye! ]\n\n"), "printf", __LINE__);
}

int
main ()
{
  atexit (PrepareToExit);
  signal (SIGINT, SigExit);

  SocketInitProcClose ();

  return 0;
}