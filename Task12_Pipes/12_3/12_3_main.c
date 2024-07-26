#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./../../Task10_Proc/10_ERRCHECKER.h"

int
main ()
{
  int str_num = 0, i = 0, slash_numb = 0;
  char enter_str[257] = { 0 }, pathname[257] = { 0 }, command[257] = { 0 },
       args[257] = { 0 }, *usr_bin_path = "/usr/bin/";

  bool need_pipe = false;

  int pipe_des[2];

  printf ("\n*[ (не) Bash ]*\n\n");

  while (1)
    {

      str_num = 0;
      slash_numb = 0;
      need_pipe = false;

      for (i = 0; i < 257; i++)
        {
          enter_str[i] = 0;
          pathname[i] = 0;
          command[i] = 0;
          args[i] = 0;
        }

      i = 0;

      while (i < 256 && (enter_str[i] = getchar ()) != '\n')
        {
          i++;
        }
      enter_str[i] = 0;

      if (strcmp (enter_str, "exit") == 0)
        {
          printf ("*Выходим...\n");
          break;
        }

      for (i = 0; i < strlen (enter_str); i++)
        {
          if (enter_str[i] == '|')
            {
              need_pipe = true;
              break;
            }
        }

      if (need_pipe == false)
        {
          if (enter_str[0] != '/' && enter_str[0] != '.')
            {
              strncpy (pathname, usr_bin_path, strlen (usr_bin_path));
              for (i = 0; i < 257; i++)
                {
                  if (enter_str[i] == ' ')
                    {
                      str_num++;

                      if (str_num != 2)
                        continue;
                      else
                        break;
                    }
                  else if (enter_str[i] == '\n' || enter_str[i] == 0)
                    {
                      break;
                    }

                  if (str_num == 0)
                    {
                      strncat (pathname, enter_str + i, 1);
                      strncat (command, enter_str + i, 1);
                    }
                  else
                    {
                      strncat (args, enter_str + i, 1);
                    }
                }
            }
          else
            {
              for (i = 0; i < 257; i++)
                {
                  if (enter_str[i] == ' ')
                    {
                      str_num++;

                      if (str_num != 2)
                        continue;
                      else
                        break;
                    }
                  else if (enter_str[i] == '\n')
                    {
                      break;
                    }
                  else if (enter_str[i] == '/')
                    {
                      slash_numb = i;
                    }

                  if (str_num < 1)
                    {
                      strncat (pathname, enter_str + i, 1);
                    }
                  else
                    {
                      if (str_num == 1)
                        {
                          for (int j = slash_numb; j < i - 1; j++)
                            {
                              strncat (command, enter_str + i, 1);
                            }

                          str_num++;
                        }

                      strncat (args, enter_str + i, 1);
                    }
                }
            }
        }
      else
        {
        }

      if (D_ERRCHECK_fork == 0)
        { // CHILD
          if ((execl (pathname, command, args, NULL)) == -1)
            {
              perror ("execl");
              exit (EXIT_FAILURE);
            }
        }
      else
        { // PARENT

          D_ERRCHECK_wait;
        }
    }

  printf ("\n*[ Bash ]*\n\n");

  return 0;
}