#include <ctype.h>
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

#include "14_2_graphic.h"

#define STRING_SIZE_MAX 255
#define WND_INPUT_FIELD_ wnd_sub_fields[WND_DOWN_INPUT]

WINDOW *windows[3], *wnd_sub_fields[3];
const int k_y_offset = 6, k_x_offset = 1;

char dump_input_string[STRING_SIZE_MAX];

// str_array_1 == uniq id
//
// str_array_1 & str_array_2 == message string & message client id
static void
OutputStringByMode (char *this_client_id_str, char *str_array_1,
                    char *str_array_2, int mode, int n_from, int n_to,
                    int cur_wnd_max_cols)
{
  if (mode == WND_RIGHT_CLIENTS_ID) // output: 1. ilya
    {
      for (int i = 0; i < n_to; i++)
        {
          mvwprintw (wnd_sub_fields[mode], i, 0, "%d. ", i + 1 + n_from);
          for (int j = 0; j < STRING_SIZE_MAX; j++)
            {
              waddch (wnd_sub_fields[mode],
                      str_array_1[(STRING_SIZE_MAX * (i + n_from)) + j]);

              // 1 усл.) перестаём печатать имя если встретили разделитель (\n)
              // или 0
              //
              // 2 усл.) нужно, чтобы имя, которое не влазиет в одну
              // строку не начало печататься на следующих строках
              if ((str_array_1[(STRING_SIZE_MAX * i) + j] == '\n'
                   || str_array_1[(STRING_SIZE_MAX * i) + j] == 0)
                  || j + 3 >= cur_wnd_max_cols - 1) // 3 = '%d' + '.' + ' '
                break;
            }
        }
    }
  else if (mode == WND_LEFT_MSGS_OF_CLIENTS) // output: ilya > hi!
    {
      for (int i = 0; i < n_to; i++)
        {
          wmove (wnd_sub_fields[mode], i, 0);

          if (strcmp (this_client_id_str,
                      (str_array_2 + (STRING_SIZE_MAX * (i + n_from))))
              != 0)
            {
              for (int j = 0; j < STRING_SIZE_MAX; j++) // output name
                {
                  if (str_array_2[(STRING_SIZE_MAX * (i + n_from)) + j]
                      == '\n')
                    break;

                  waddch (wnd_sub_fields[mode],
                          str_array_2[(STRING_SIZE_MAX * (i + n_from)) + j]);
                }

              wprintw (wnd_sub_fields[mode], " > ");
            }
          else
            {
              wprintw (wnd_sub_fields[mode], "<< ");
            }

          for (int j = 0; j < STRING_SIZE_MAX; j++) // output msg
            {
              if (str_array_1[(STRING_SIZE_MAX * (i + n_from)) + j] == 0)
                break;

              waddch (wnd_sub_fields[mode],
                      str_array_1[(STRING_SIZE_MAX * (i + n_from)) + j]);
            }

          waddch (wnd_sub_fields[mode], '\n');
        }
    }
}

// str_array_1 == uniq id
//
// str_array_1 & str_array_2 == message string & message client id
void
OutputStringArrayToWindowNum (char *this_client_id_str, char *str_array_1,
                              char *str_array_2, int str_count, int wnd_num)
{
  int cur_wnd_max_rows = getmaxy (wnd_sub_fields[wnd_num]);
  int cur_wnd_max_cols = getmaxx (wnd_sub_fields[wnd_num]);

  int n_to = 0, n_from = 0;

  if (str_count < cur_wnd_max_rows)
    {
      n_from = 0;
      n_to = str_count;
    }
  else
    {
      n_from = str_count - cur_wnd_max_rows;
      n_to = cur_wnd_max_rows;
    }

  werase (wnd_sub_fields[wnd_num]);
  wmove (wnd_sub_fields[wnd_num], 0, 0);

  OutputStringByMode (this_client_id_str, str_array_1, str_array_2, wnd_num,
                      n_from, n_to, cur_wnd_max_cols);

  wrefresh (wnd_sub_fields[wnd_num]);

  // return cursor to input field
  wmove (WND_INPUT_FIELD_, 0, 0);
  wrefresh (WND_INPUT_FIELD_);
}

static void
DumpInputStringToScreen ()
{
  werase (WND_INPUT_FIELD_);
  wmove (WND_INPUT_FIELD_, 0, 0);

  for (int j = 0; j < STRING_SIZE_MAX; j++)
    {

      if (dump_input_string[j] == 0)
        break;

      waddch (WND_INPUT_FIELD_, dump_input_string[j]);
    }
}

char *
InputHandler (int input_mode)
{

  werase (WND_INPUT_FIELD_);
  wmove (WND_INPUT_FIELD_, 0, 0);
  wrefresh (WND_INPUT_FIELD_);

  int buffer_char = 0, i = 0;

  for (int j = 0; j < STRING_SIZE_MAX; j++)
    {

      if (dump_input_string[j] == 0)
        break;

      dump_input_string[j] = 0;
    }

  while (i < STRING_SIZE_MAX / 2)
    {
      buffer_char = wgetch (WND_INPUT_FIELD_); // get user input

      if (isprint (buffer_char) != 0)
        {
          if (isgraph (buffer_char) == 0 && input_mode == INPUT_MODE_LOGIN)
            break;

          waddch (WND_INPUT_FIELD_, buffer_char);
          dump_input_string[i] = (char)buffer_char;

          i++;
        }
      else
        {
          if (buffer_char == '\n')
            {
              break;
            }

          if (buffer_char == KEY_BACKSPACE && i != 0)
            {
              i--;
              dump_input_string[i] = 0;

              DumpInputStringToScreen ();
            }
          else if (buffer_char == KEY_F (10))
            {
              return ((void *)-1); // *exiting*
            }
        }

      wrefresh (WND_INPUT_FIELD_);
    }

  werase (WND_INPUT_FIELD_);
  wrefresh (WND_INPUT_FIELD_);

  return dump_input_string;
}

static void
ResizeWindow (int wnd_num)
{

  const int percent_of_col_size = 40;

  int stdscr_row_size, stdscr_col_size;
  int resize_col_size;

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

          wnd_sub_fields[wnd_num]
              = newwin (stdscr_row_size - k_y_offset - 2, resize_col_size - 2,
                        1, (stdscr_col_size - resize_col_size) * wnd_num + 1);
        }
      else
        {
          wresize (windows[wnd_num], stdscr_row_size - k_y_offset - 2,
                   resize_col_size - 2);
          mvwin (windows[wnd_num], 1,
                 (stdscr_col_size - resize_col_size) * wnd_num + 1);

          wresize (wnd_sub_fields[wnd_num], stdscr_row_size - k_y_offset - 2,
                   resize_col_size - 2);
          mvwin (wnd_sub_fields[wnd_num], 1,
                 (stdscr_col_size - resize_col_size) * wnd_num + 1);
        }
    }
  else
    {
      if (windows[wnd_num] == NULL)
        {
          windows[wnd_num] = newwin (k_y_offset - 1, stdscr_col_size,
                                     stdscr_row_size - k_y_offset + 1, 0);
          wnd_sub_fields[wnd_num]
              = newwin (k_y_offset - 1 - 2, stdscr_col_size - 2,
                        stdscr_row_size - k_y_offset + 1 + 1, 1);
        }
      else
        {
          wresize (windows[wnd_num], k_y_offset - 2, stdscr_row_size - 2);
          mvwin (windows[wnd_num], stdscr_row_size - k_y_offset + 1, 1);
        }
    }
}

static void
RenderWindow (int wnd_num)
{
  werase (windows[wnd_num]);

  ResizeWindow (wnd_num);

  wattron (windows[wnd_num], COLOR_PAIR (1));

  if (wnd_num != 3)
    box (windows[wnd_num], ACS_VLINE, ACS_HLINE);

  wrefresh (windows[wnd_num]);
  wrefresh (wnd_sub_fields[wnd_num]);
}

static void
DeleteWindows ()
{
  for (int i = 0; i < 3; i++)
    {
      delwin (windows[i]);
      delwin (wnd_sub_fields[i]);
    }
}

static void
RenderScreen ()
{

  erase ();
  refresh ();

  RenderWindow (0);
  RenderWindow (1);
  RenderWindow (2);

  keypad (WND_INPUT_FIELD_, TRUE);
  intrflush (wnd_sub_fields[WND_DOWN_INPUT], TRUE);
}

void
SignalGraphicExit ()
{
  DeleteWindows ();
  endwin ();
}

void
InitGraphic ()
{
  setlocale (LC_ALL, "");

  initscr ();
  cbreak ();
  noecho ();

  signal (SIGINT, SignalGraphicExit);

  start_color ();
  init_pair (1, COLOR_WHITE, COLOR_BLACK);
  init_pair (2, COLOR_BLUE, COLOR_BLACK);

  refresh ();

  RenderScreen ();

  //   wgetch (wnd_sub_fields[WND_DOWN_INPUT]);

  /* wrefresh (windows[wnd_num]);
wrefresh (fields[wnd_num]);*/

  //   SignalWindowResize (0);//
}