#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int
main ()
{
  int fifo_file;

  char str_wr[3] = "Hi!";
  char *fifo_path = "./fifo_path_server";

  mkfifo (fifo_path, S_IRUSR | S_IWUSR);

  fifo_file = open (fifo_path, O_WRONLY);
  write (fifo_file, str_wr, 3 * sizeof (char));

  exit (EXIT_SUCCESS);
}
