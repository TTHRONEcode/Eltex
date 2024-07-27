#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./../../Task10_Proc/10_ERRCHECKER.h"
#include "./12_3_command.h"

int str_num, g_i, slash_numb, pipe_count;
char enter_str[257], pathname[2][257], command[2][257], args[2][257],
    *usr_bin_path = "/usr/bin/";

int pipe_pos;
int pipe_des[2];

void
CommandConstruct (int comm_n, int from_n, int to_n)
{
  str_num = 0;

  if (enter_str[from_n] != '/' && enter_str[from_n] != '.')
    {
      strncpy (pathname[comm_n], usr_bin_path, strlen (usr_bin_path));
      for (g_i = from_n; g_i < to_n; g_i++)
        {
          if (enter_str[g_i] == ' ')
            {
              str_num++;

              if (str_num != 2)
                continue;
              else
                break;
            }
          else if (enter_str[g_i] == '\n' || enter_str[g_i] == 0)
            {
              break;
            }

          if (str_num == 0)
            {
              strncat (pathname[comm_n], enter_str + g_i, 1);
              strncat (command[comm_n], enter_str + g_i, 1);
            }
          else
            {
              strncat (args[comm_n], enter_str + g_i, 1);
            }
        }
    }
  else
    {
      for (g_i = from_n; g_i < to_n; g_i++)
        {
          if (enter_str[g_i] == ' ')
            {
              str_num++;

              if (str_num != 2)
                continue;
              else
                break;
            }
          else if (enter_str[g_i] == '\n')
            {
              break;
            }
          else if (enter_str[g_i] == '/')
            {
              slash_numb = g_i;
            }

          if (str_num < 1)
            {
              strncat (pathname[comm_n], enter_str + g_i, 1);
            }
          else
            {
              if (str_num == 1)
                {
                  for (int j = slash_numb; j < g_i - 1; j++)
                    {
                      strncat (command[comm_n], enter_str + g_i, 1);
                    }

                  str_num++;
                }

              strncat (args[comm_n], enter_str + g_i, 1);
            }
        }
    }
}

void
CommandExecute (int p_count)
{
  if (p_count == 0)
    {
      CommandConstruct (0, 0, 257);

      if (D_ERRCHECK_fork == 0)
        { // CHILD
          if ((execl (pathname[0], command[0], args[0], NULL)) == -1)
            {
              perror ("execl _0");
              exit (EXIT_FAILURE);
            }
        }
      else
        { // PARENT
          D_ERRCHECK_wait;
        }
    }
  else
    {
      CommandConstruct (0, 0, pipe_pos - 1);
      CommandConstruct (1, pipe_pos + 2, 257);

      if (pipe (pipe_des) == -1)
        {
          perror ("pipe");
          exit (EXIT_FAILURE);
        }

      if (D_ERRCHECK_fork == 0)
        { // CHILD
          dup2 (pipe_des[1], 1);
          if ((execl (pathname[0], command[0], args[0], NULL)) == -1)
            {
              perror ("execl _1");
              exit (EXIT_FAILURE);
            }
        }
      else
        { // PARENT
          D_ERRCHECK_wait;

          if (D_ERRCHECK_fork == 0)
            { // CHILD
              dup2 (pipe_des[0], 0);
              if ((execl (pathname[1], command[1], args[1], NULL)) == -1)
                {
                  perror ("execl _2");
                  exit (EXIT_FAILURE);
                }
            }
          else
            { // PARENT
              D_ERRCHECK_wait; // grep почему-то не завершается вообще, а
                               // просто до бесконечности ожидает ввода.
                               // Что нужно сделать?
            }
        }
    }
}

void
CommandPrepare ()
{
  while (1)
    {
      slash_numb = 0;
      pipe_count = 0;
      pipe_pos = 0;

      for (g_i = 0; g_i < 2; g_i++)
        {
          for (int j = 0; j < 257; j++)
            {
              enter_str[j] = 0;
              pathname[g_i][j] = 0;
              command[g_i][j] = 0;
              args[g_i][j] = 0;
            }
        }

      g_i = 0;

      printf ("(не) Bash > ");
      while (g_i < 256 && (enter_str[g_i] = getchar ()) != '\n')
        {
          g_i++;
        }

      enter_str[g_i] = 0;

      if (strcmp (enter_str, "exit") == 0)
        {
          printf ("*Выходим...\n");
          break;
        }
      printf ("\n");

      for (g_i = 0; g_i < strlen (enter_str); g_i++)
        {
          if (enter_str[g_i] == '|')
            {
              pipe_count++;
              pipe_pos = g_i; //

              break;
            }
        }

      CommandExecute (pipe_count);
    }
}