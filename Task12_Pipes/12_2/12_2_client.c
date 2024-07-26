#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int
main ()
{
  int fifo_file;

  char str_r[3] = { 0 };
  char *fifo_path = "./fifo_path_server";

  fifo_file = open (fifo_path, O_RDONLY);
  read (fifo_file, str_r, 3 * sizeof (char));
  close (fifo_file);

  printf ("%s\n", str_r);

  if (unlink (fifo_path) != 0)
    {
      perror ("unlink");
      return 1;
    }

  exit (EXIT_SUCCESS);
}
