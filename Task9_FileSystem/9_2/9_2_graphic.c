#include "9_2_data_get.h"
#include <dirent.h>
#include <malloc.h>
#include <menu.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>

struct dirent **full_dp;

WINDOW *headers_wnd[3], *windows_wnd[2];
char *header_main_str = "TotalBlack Commander";
char *cur_dir_left = ".../home/Doc/";
int dp_count = 0;
static void SigWinCh(int signo) {
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
  resizeterm(size.ws_row, size.ws_col);
}

static void InitWnds() {
  int row, col;
  WINDOW *case_wnd = NULL;

  // Center
  getmaxyx(stdscr, row, col);
  case_wnd = newwin(row, col, 0, 0);
  wattron(case_wnd, COLOR_PAIR(1));
  mvwprintw(case_wnd, 0, (col - strlen(header_main_str) - 1) / 2 - 1, " %s ",
            header_main_str);
  headers_wnd[0] = case_wnd;
  ////////

  // Left Block
  getmaxyx(headers_wnd[0], row, col);
  case_wnd = derwin(headers_wnd[0], row - 1, (col) / 2, 1, 0);
  wattron(case_wnd, COLOR_PAIR(1));
  box(case_wnd, ACS_VLINE, ACS_HLINE);
  windows_wnd[0] = case_wnd;
  // left header
  getmaxyx(windows_wnd[0], row, col);
  case_wnd = derwin(windows_wnd[0], 1, col, 0, 0);
  wattron(case_wnd, COLOR_PAIR(1) | A_REVERSE);
  mvwprintw(case_wnd, 0, (col - strlen(cur_dir_left) - 1) / 2 - 1, " %s ",
            cur_dir_left);
  headers_wnd[1] = case_wnd;
  ////////

  // Right
  getmaxyx(headers_wnd[0], row, col);
  case_wnd = derwin(headers_wnd[0], row - 1, (col) / 2 + 1, 1, col / 2 - 1);
  wattron(case_wnd, COLOR_PAIR(1));
  box(case_wnd, ACS_VLINE, ACS_HLINE);
  windows_wnd[1] = case_wnd;
  // right header
  getmaxyx(windows_wnd[1], row, col);
  case_wnd = derwin(windows_wnd[1], 1, col, 0, 0);
  wattron(case_wnd, COLOR_PAIR(1));
  mvwprintw(case_wnd, 0, (col - strlen(cur_dir_left) - 1) / 2 - 1, " %s ",
            cur_dir_left);
  headers_wnd[2] = case_wnd;
  ////////

  // refresh
  for (int i = 0; i < 3; i++) {
    if (i < 2)
      wrefresh(windows_wnd[i]);

    wrefresh(headers_wnd[i]);
  }
}

void ChHeader(int num, char *ptr_head) {
  WINDOW *loc_header = headers_wnd[num + 1];
  int col = getmaxx(loc_header);
  int percent = (col * 75 / 100);
  char *head_str = NULL;

  head_str = calloc(strlen(ptr_head), 1);

  strncpy(head_str, ptr_head + (strlen(ptr_head) - percent), percent);

  if (strlen(ptr_head) > percent) {
    for (int i = 0; i < 3; i++) {
      head_str[i] = '.';
    }
  }

  wattron(loc_header, COLOR_PAIR(1) | A_BOLD);
  mvwprintw(loc_header, 0, (col - strlen(head_str) - 1) / 2, " %s ", head_str);

  // refresh();
  wrefresh(loc_header);
  free(head_str);
}

int MenuManager() {
  int row, col, cur_dp_id = 0, end = 0;

  int c = 0;
  MENU *my_menu = NULL;

  getmaxyx(windows_wnd[0], row, col);

  ITEM **my_item = NULL;

  my_item = (ITEM **)calloc(dp_count + 1, sizeof(ITEM *));

  char **full_item_str = calloc(dp_count, sizeof(char *));

  for (int i = 1; i < dp_count; i++) {
    full_item_str[i] = calloc(strlen(full_dp[i]->d_name) + 2, sizeof(char));

    full_item_str[i][0] = full_dp[i]->d_type == DT_DIR ? '/' : ' ';
    strcat(full_item_str[i], full_dp[i]->d_name);

    my_item[i - 1] = new_item(full_item_str[i], "");
  }
  my_item[dp_count] = NULL;

  my_menu = new_menu((ITEM **)my_item);
  keypad(windows_wnd[0], TRUE);
  set_menu_win(my_menu, windows_wnd[0]);
  set_menu_sub(my_menu, derwin(windows_wnd[0], row - 1, col - 1, 1, 1));
  set_menu_mark(my_menu, "");
  post_menu(my_menu);

  wrefresh(windows_wnd[0]);

  while (c != 'q' && end != 1) {
    c = getch();
    switch (c) {
    case KEY_DOWN:
      menu_driver(my_menu, REQ_DOWN_ITEM);
      if (cur_dp_id < dp_count - 2)
        cur_dp_id++;
      break;
    case KEY_UP:
      menu_driver(my_menu, REQ_UP_ITEM);
      if (cur_dp_id > 0)
        cur_dp_id--;
      break;

    case 'e':
      if (full_dp[cur_dp_id + 1]->d_type == DT_DIR) {
        EnterDir(full_dp[cur_dp_id + 1]->d_name);
        end = 1;
      }
      break;
    }

    if (end != 1)
      wrefresh(windows_wnd[0]);
  }

  free_item(my_item[0]);
  free_item(my_item[1]);
  free_menu(my_menu);

  free(full_item_str);
  free(my_item);

  if (end != 1) {
    endwin();
    exit(EXIT_SUCCESS);
  } else {
    return end;
  }
}

void ChList(int num, int count, struct dirent ***list, char *cur_dir) {
  WINDOW *loc_window = windows_wnd[num];

  full_dp = *list;
  dp_count = count;

  werase(loc_window);

  wattron(loc_window, COLOR_PAIR(1) | A_BOLD);
  box(loc_window, ACS_VLINE, ACS_HLINE);

  // refresh();

  // wrefresh(loc_window);
  ChHeader(num, cur_dir);
}

void GraphicShow() {
  initscr();
  signal(SIGWINCH, SigWinCh);
  cbreak();

  curs_set(FALSE);
  start_color();

  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);

  noecho();

  keypad(stdscr, TRUE);

  refresh();
  InitWnds();
}
