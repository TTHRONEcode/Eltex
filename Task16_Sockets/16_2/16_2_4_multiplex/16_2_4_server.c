#include <err.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "./16_2_4.h"

struct sockaddr_in sockaddr_serv, client;
int sock_tcp_fd, sock_udp_fd;
socklen_t sockaddr_len = sizeof (struct sockaddr);

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

void *
ThreadSocketClient (void *args)
{
  char msg_recv[STR_SIZE_MAX] = { 0 };
  int sub_server_fd = 0;
  struct sockaddr_in sub_server = { 0 };
  socklen_t loc_sockaddr_len = sizeof (struct sockaddr);

  int loc_client_fd = ((int *)args)[0], this_thread_num = ((int *)args)[1],
      sock_mode = ((int *)args)[2];

  CheckError (printf ("\n*Thread №%d is getting started with fd %d*\n"
                      "*Socket mode: %s*\n",
                      this_thread_num, loc_client_fd,
                      k_socket_modes[sock_mode]),
              "printf", __LINE__);

  if (sock_mode == SOCK_MODE_UDP)
    {
      sub_server.sin_family = AF_INET;
      sub_server.sin_port = htons (k_server_main_port + this_thread_num);
      sub_server.sin_addr.s_addr = INADDR_ANY;

      // настраиваем соединение
      CheckError (sub_server_fd
                  = socket (sub_server.sin_family, SOCK_DGRAM, 0),
                  "socket", __LINE__);
      CheckError (bind (sub_server_fd, (struct sockaddr *)&sub_server,
                        loc_sockaddr_len),
                  "bind", __LINE__);

      CheckError (sendto (loc_client_fd, &this_thread_num, loc_sockaddr_len, 0,
                          (struct sockaddr *)&client, loc_sockaddr_len),
                  "recvfrom", __LINE__);
    }

  while (1)
    {
      if (sock_mode == SOCK_MODE_TCP)
        {
          CheckError (recv (loc_client_fd, (char *)msg_recv, STR_SIZE_MAX, 0),
                      "recv", __LINE__);
        }
      else if (sock_mode == SOCK_MODE_UDP)
        {

          CheckError (
              recvfrom (sub_server_fd, (char *)msg_recv, loc_sockaddr_len, 0,
                        (struct sockaddr *)&sub_server, &loc_sockaddr_len),
              "recvfrom", __LINE__);
        }

      if (msg_recv[0] == k_MSG_T_GIVE_ME_TIME)
        {
          time_t time_not_str = time (NULL);
          char *time_to_str = asctime (gmtime (&time_not_str));

          if (sock_mode == SOCK_MODE_TCP)
            {
              CheckError (
                  send (loc_client_fd, (char *)time_to_str, STR_SIZE_MAX, 0),
                  "send", __LINE__);
            }
          else if (sock_mode == SOCK_MODE_UDP)
            {
              CheckError (sendto (sub_server_fd, time_to_str, loc_sockaddr_len,
                                  0, (struct sockaddr *)&sub_server,
                                  loc_sockaddr_len),
                          "recvfrom", __LINE__);
            }
        }
      else
        {
          break;
        }

      CheckError (
          printf ("*%d. The request has been successfully processed*\n",
                  this_thread_num),
          "printf", __LINE__);
    }

  CheckError (printf ("*Thread №%d is exiting*\n", this_thread_num), "printf",
              __LINE__);

  if (sock_mode == SOCK_MODE_TCP)
    CheckError (close (loc_client_fd), "close", __LINE__);
  else if (sock_mode == SOCK_MODE_UDP)
    CheckError (close (sub_server_fd), "close", __LINE__);

  return NULL;
}

static void
SocketsHandler (int loc_sock_tcp_fd, int loc_sock_udp_fd)
{
  char message_buf[STR_SIZE_MAX] = { 0 };
  int threads_count = 0;
  int client_tcp_fd = 0;
  int sock_mode = 0;
  int pool_ret = 0;

  struct pollfd poll_fds[2];

  poll_fds[SOCK_MODE_UDP].fd = loc_sock_udp_fd;
  poll_fds[SOCK_MODE_TCP].fd = loc_sock_tcp_fd;

  poll_fds[SOCK_MODE_UDP].events = POLLIN;
  poll_fds[SOCK_MODE_TCP].events = POLLIN;

  while (1)
    {
      CheckError (printf ("[ MAIN THREAD: Wait for client ]\n"), "printf",
                  __LINE__);

      pool_ret = 0;
      pool_ret = poll (poll_fds, 2, -1);
      if (pool_ret == EINVAL)
        break;

      threads_count++;

      // UDP wait
      if (poll_fds[SOCK_MODE_UDP].revents & POLLIN)
        {

          CheckError (recvfrom (loc_sock_udp_fd, message_buf, STR_SIZE_MAX, 0,
                                (struct sockaddr *)&client, &sockaddr_len),
                      "recvfrom", __LINE__);

          sock_mode = SOCK_MODE_UDP;
        }
      // TCP wait
      else if (poll_fds[SOCK_MODE_TCP].revents & POLLIN)
        {
          printf ("[ POLL: SOCK_MODE_TCP]\n");

          CheckError (client_tcp_fd
                      = accept (loc_sock_tcp_fd, (struct sockaddr *)&client,
                                &sockaddr_len),
                      "accept", __LINE__);

          sock_mode = SOCK_MODE_TCP;
        }

      CheckError (printf ("[ MAIN THREAD: Client connected with %s mode ]\n\n",
                          k_socket_modes[sock_mode]),
                  "printf", __LINE__);

      int pthread_params[]
          = { (sock_mode == SOCK_MODE_UDP ? loc_sock_udp_fd : client_tcp_fd),
              threads_count, sock_mode };

      pthread_t thread = 0;
      CheckError (thread = pthread_create (&thread, NULL, ThreadSocketClient,
                                           (void *)pthread_params),
                  "pthread_create", __LINE__);
    }
}

static void
SocketInitProcClose ()
{
  CheckError (printf ("-> HELP: Want to exit? Press Ctrl-C! <-\n\n"), "printf",
              __LINE__);

  sockaddr_serv.sin_family = AF_INET;
  sockaddr_serv.sin_port = htons (k_server_main_port);
  sockaddr_serv.sin_addr.s_addr = INADDR_ANY;

  // TCP
  CheckError (sock_tcp_fd = socket (sockaddr_serv.sin_family, SOCK_STREAM, 0),
              "socket", __LINE__);
  CheckError (
      bind (sock_tcp_fd, (struct sockaddr *)&sockaddr_serv, sockaddr_len),
      "bind", __LINE__);
  CheckError (listen (sock_tcp_fd, 2), "listen", __LINE__);

  // UDP
  CheckError (sock_udp_fd = socket (sockaddr_serv.sin_family, SOCK_DGRAM, 0),
              "socket", __LINE__);
  CheckError (
      bind (sock_udp_fd, (struct sockaddr *)&sockaddr_serv, sockaddr_len),
      "bind", __LINE__);

  SocketsHandler (sock_tcp_fd, sock_udp_fd);

  CheckError (close (sock_tcp_fd), "close", __LINE__);
  CheckError (close (sock_udp_fd), "close", __LINE__);
}

void
SigHandler (int sig_num __attribute__ ((unused)),
            siginfo_t *info __attribute__ ((unused)),
            void *args __attribute__ ((unused)))
{
  printf ("\n[ MAIN THREAD: SIGINT ]\n\n");
}

static void
PrepareToExit ()
{
  CheckError (printf ("\n[ MAIN THREAD: preparing to exit... ]\n"), "printf",
              __LINE__);

  CheckError (printf ("[ MAIN THREAD: closing sock_*_fd... ]\n"), "printf",
              __LINE__);

  CheckError (close (sock_tcp_fd), "close", __LINE__);
  CheckError (close (sock_udp_fd), "close", __LINE__);

  CheckError (printf ("[ MAIN THREAD: bye-bye! ]\n\n"), "printf", __LINE__);
}

static void
InitSigHandler ()
{
  struct sigaction sig_handler;
  sigset_t sig_set;

  sigemptyset (&sig_set);
  sigaddset (&sig_set, SIGINT);
  sig_handler.sa_sigaction = SigHandler;
  sig_handler.sa_mask = sig_set;
  sig_handler.sa_flags = 0;

  if (sigaction (SIGINT, &sig_handler, NULL) < 0)
    PrintErrorAndExit ("sigaction", __LINE__);
}

int
main ()
{
  if (atexit (PrepareToExit) != 0)
    PrintErrorAndExit ("atexit", __LINE__);

  InitSigHandler ();

  SocketInitProcClose ();

  return 0;
}