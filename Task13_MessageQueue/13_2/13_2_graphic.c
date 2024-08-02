#include <curses.h>
#include <dirent.h>
#include <err.h>
#include <locale.h>
#include <malloc.h>
#include <menu.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>

WINDOW *windows[3], *fields[3];
const int k_y_offset = 6, k_x_offset = 1;
int only_one;

extern char **other_clients_msgs, **other_clients_msgs_names, **other_id;
extern int other_clients_msgs_count, clients_count;

void
RenderIds ()
{
  int max_rows = getmaxy (fields[1]);
  int n_to = 0, n_from = 0;

  werase (fields[1]);

  if (clients_count < max_rows)
    {
      n_from = 0;
      n_to = clients_count;
    }
  else
    {
      n_from = clients_count - max_rows;
      n_to = max_rows;
    }

  for (int j = 0; j < n_to; j++)
    {
      mvwprintw (fields[1], j, 0, "%d) %s", j + n_from, other_id[j + n_from]);
    }
  wmove (fields[2], 0, 0);
  wrefresh (fields[1]);
  refresh ();
}

void
RenderInputMsgs (int is_this)
{
  int max_rows = getmaxy (fields[0]);
  int n_to = 0, n_from = 0;

  werase (fields[0]);

  if (other_clients_msgs_count < max_rows)
    {
      n_from = 0;
      n_to = other_clients_msgs_count;
    }
  else
    {
      n_from = other_clients_msgs_count - max_rows;
      n_to = max_rows;
    }

  for (int j = 0; j < n_to; j++)
    {
      mvwprintw (fields[0], j, 0, "%s > %s",
                 other_clients_msgs_names[j + n_from],
                 other_clients_msgs[j + n_from]);
    }
  wmove (fields[2], 0, 0);
  wrefresh (fields[0]);
}

void
RedrawInputField ()
{

  redrawwin (windows[2]);
  wrefresh (windows[2]);

  werase (fields[2]);
  wrefresh (fields[2]);
}

void
PrintThisId (char *id)
{
  mvwprintw (stdscr, getmaxy (stdscr) - k_y_offset, 0, "%s\n", id);
  refresh ();
  RedrawInputField ();
}

void
EnterStdinString (int mode, char *ent_str)
{
  int strlen_for = strlen (ent_str);
  for (int i = 0; i < strlen_for; i++)
    {
      ent_str[i] = 0;
    }

  int g_i = 0, getch_i = 0;
  while (g_i < (255 + 2) / 2)
    {
      getch_i = wgetch (fields[2]);

      if (getch_i == KEY_BACKSPACE)
        {

          if (g_i > 0)
            {
              wdelch (fields[2]);
              ent_str[g_i] = 0;
              g_i--;
            }
        }
      else
        {
          if (only_one == 0)
            {
              only_one = 1;
              continue;
            }

          ent_str[g_i] = (char)getch_i;

          if (ent_str[g_i] == '\n' || (mode == 0 && ent_str[g_i] == ' '))
            {
              if (mode == 0 && ent_str[g_i] == ' ')
                {
                  ent_str[g_i] = 0;
                }
              break;
            }
          g_i++;
        }
    }

  ent_str[g_i] = 0;
  RedrawInputField ();
}

void
PutLoginText ()
{
  int stdscr_row_size = getmaxy (stdscr);
  mvwprintw (stdscr, stdscr_row_size - k_y_offset, k_x_offset, "Enter Login:");

  wmove (fields[2], 0, 0);

  refresh ();
}

void
ResizeWindow (int wnd_num)
{

  const int percent_of_col_size = 40;

  int stdscr_row_size, stdscr_col_size;
  int resize_row_size, resize_col_size;

  getmaxyx (stdscr, stdscr_row_size, stdscr_col_size);

  resize_col_size
      = (stdscr_col_size
         * ((wnd_num == 0 ? (100 - percent_of_col_size) : percent_of_col_size)
            + wnd_num)
         / 100);

  if (wnd_num < 2)
    {
      if (windows[wnd_num] == NULL)
        {
          windows[wnd_num]
              = newwin (stdscr_row_size - k_y_offset, resize_col_size, 0,
                        (stdscr_col_size - resize_col_size) * wnd_num);

          fields[wnd_num]
              = newwin (stdscr_row_size - k_y_offset - 2, resize_col_size - 2,
                        1, (stdscr_col_size - resize_col_size) * wnd_num + 1);
        }
      else
        {
          wresize (windows[wnd_num], stdscr_row_size - k_y_offset - 2,
                   resize_col_size - 2);
          mvwin (windows[wnd_num], 1,
                 (stdscr_col_size - resize_col_size) * wnd_num + 1);

          wresize (fields[wnd_num], stdscr_row_size - k_y_offset - 2,
                   resize_col_size - 2);
          mvwin (fields[wnd_num], 1,
                 (stdscr_col_size - resize_col_size) * wnd_num + 1);
        }
    }
  else
    {
      if (windows[wnd_num] == NULL)
        {
          windows[wnd_num] = newwin (k_y_offset - 1, stdscr_col_size,
                                     stdscr_row_size - k_y_offset + 1, 0);
          fields[wnd_num] = newwin (k_y_offset - 1 - 2, stdscr_col_size - 2,
                                    stdscr_row_size - k_y_offset + 1 + 1, 1);
        }
      else
        {
          wresize (windows[wnd_num], k_y_offset - 2, stdscr_row_size - 2);
          mvwin (windows[wnd_num], stdscr_row_size - k_y_offset + 1, 1);
        }
    }
}

void
RenderWindow (int wnd_num)
{

  werase (windows[wnd_num]);

  ResizeWindow (wnd_num);

  wattron (windows[wnd_num], COLOR_PAIR (1));

  if (wnd_num != 3)
    box (windows[wnd_num], ACS_VLINE, ACS_HLINE);

  wrefresh (windows[wnd_num]);
  wrefresh (fields[wnd_num]);
}

void
DelWins ()
{
  for (int i = 0; i < 3; i++)
    {
      delwin (windows[i]);
      delwin (fields[i]);
    }
}

void
RenderScreen ()
{

  erase ();
  refresh ();

  RenderWindow (0);
  RenderWindow (1);
  RenderWindow (2);

  keypad (fields[2], TRUE);
  intrflush (fields[2], TRUE);
}

void
SignalWindowResize (int signo)
{

  struct winsize wnd_size;
  ioctl (fileno (stdout), TIOCGWINSZ, (char *)&wnd_size);
  resizeterm (wnd_size.ws_row, wnd_size.ws_col);

  RenderScreen ();
}

void
SignalExit (int signo)
{
  endwin ();
  exit (EXIT_SUCCESS);
}

void
InitGraphic ()
{
  setlocale (LC_ALL, "");

  initscr ();
  cbreak ();
  echo ();

  signal (SIGWINCH, SignalWindowResize);
  signal (SIGINT, SignalExit);

  start_color ();
  init_pair (1, COLOR_WHITE, COLOR_BLACK);
  init_pair (2, COLOR_BLUE, COLOR_BLACK);

  SignalWindowResize (0);
}
