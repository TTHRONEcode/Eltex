#include <curses.h>
#include <dirent.h>
#include <err.h>
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

WINDOW *windows[2];

void
ResizeWindow (int wnd_num)
{

  const int percent_of_col_size = 25, row_offset = 3;

  int stdscr_row_size, stdscr_col_size;
  int resize_row_size, resize_col_size;

  getmaxyx (stdscr, stdscr_row_size, stdscr_col_size);

  resize_col_size
      = (stdscr_col_size
         * ((wnd_num == 0 ? (100 - percent_of_col_size) : percent_of_col_size)
            + wnd_num)
         / 100);

  if (windows[wnd_num] == NULL)
    {
      windows[wnd_num] = newwin (
          stdscr_row_size - ((1 - wnd_num) * row_offset), resize_col_size, 0,
          (stdscr_col_size - resize_col_size) * wnd_num);
    }
  else
    {
      wresize (windows[wnd_num],
               stdscr_row_size - ((1 - wnd_num) * row_offset),
               resize_col_size);
      mvwin (windows[wnd_num], 0,
             (stdscr_col_size - resize_col_size) * wnd_num);
    }
}

void
RenderWindow (int wnd_num)
{

  werase (windows[wnd_num]);

  ResizeWindow (wnd_num);

  wattron (windows[wnd_num], COLOR_PAIR (1));
  box (windows[wnd_num], ACS_VLINE, ACS_HLINE);

  wrefresh (windows[wnd_num]);
}

void
RenderScreen ()
{
  const int x_offset = 3, y_pos = 1;

  erase ();
  refresh ();

  RenderWindow (0);
  RenderWindow (1);

  move (getmaxy (stdscr) - x_offset, y_pos);
  refresh ();
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
  initscr ();
  signal (SIGWINCH, SignalWindowResize);
  signal (SIGINT, SignalExit);
  //   noecho ();

  // curs_set (FALSE);

  start_color ();
  init_pair (1, COLOR_WHITE, COLOR_BLACK);
  init_pair (2, COLOR_BLUE, COLOR_BLACK);

  keypad (stdscr, TRUE);

  SignalWindowResize (0);
  //   getch ();
}
