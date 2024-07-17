#include "9_2_data_get.h"
#include <curses.h>
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

struct dirent **g_dir_lists[2];

WINDOW *g_headers_wnd[3], *g_windows_wnd[2];
const char *g_header_main_str = "TotalBlack Commander";
char *g_cur_dir_left = "aaaaaaaaa"; // TODO remove
int g_list_el_count = 0;
static void SigWinCh(int p_signo) {
  struct winsize l_size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&l_size);
  resizeterm(l_size.ws_row, l_size.ws_col);
}

static void InitWnds() {
  int row, col;
  WINDOW *case_wnd = NULL;

  // Center
  getmaxyx(stdscr, row, col);
  case_wnd = newwin(row, col, 0, 0);
  wattron(case_wnd, COLOR_PAIR(1));
  mvwprintw(case_wnd, 0, (col - strlen(g_header_main_str) - 1) / 2 - 1, " %s ",
            g_header_main_str);
  g_headers_wnd[0] = case_wnd;
  ////////

  // Left Block
  getmaxyx(g_headers_wnd[0], row, col);
  case_wnd = derwin(g_headers_wnd[0], row - 1, (col) / 2, 1, 0);
  wattron(case_wnd, COLOR_PAIR(1));
  box(case_wnd, ACS_VLINE, ACS_HLINE);
  g_windows_wnd[0] = case_wnd;
  // left header
  getmaxyx(g_windows_wnd[0], row, col);
  case_wnd = derwin(g_windows_wnd[0], 1, col, 0, 0);
  wattron(case_wnd, COLOR_PAIR(1) | A_REVERSE);
  mvwprintw(case_wnd, 0, (col - strlen(g_cur_dir_left) - 1) / 2 - 1, " %s ",
            g_cur_dir_left);
  g_headers_wnd[1] = case_wnd;
  ////////

  // Right
  getmaxyx(g_headers_wnd[0], row, col);
  case_wnd = derwin(g_headers_wnd[0], row - 1, (col) / 2 + 1, 1, col / 2 - 1);
  wattron(case_wnd, COLOR_PAIR(1));
  box(case_wnd, ACS_VLINE, ACS_HLINE);
  g_windows_wnd[1] = case_wnd;
  // right header
  getmaxyx(g_windows_wnd[1], row, col);
  case_wnd = derwin(g_windows_wnd[1], 1, col, 0, 0);
  wattron(case_wnd, COLOR_PAIR(1));
  mvwprintw(case_wnd, 0, (col - strlen(g_cur_dir_left) - 1) / 2 - 1, " %s ",
            g_cur_dir_left);
  g_headers_wnd[2] = case_wnd;
  ////////

  // refresh
  for (int i = 0; i < 3; i++) {
    if (i < 2)
      wrefresh(g_windows_wnd[i]);

    wrefresh(g_headers_wnd[i]);
  }
}

void ChHeaderDirStr(int p_num, char *p_str_in) {
  WINDOW *l_loc_header = g_headers_wnd[1 + p_num];
  int l_col = getmaxx(l_loc_header);
  int l_max_head_size = (l_col * 75 / 100);
  int l_dir_str_size = strlen(p_str_in);
  char *l_str_out = calloc(l_max_head_size, sizeof(char));

  if (l_dir_str_size > l_max_head_size) {
    strncpy(l_str_out, p_str_in + (l_dir_str_size - l_max_head_size),
            l_max_head_size);

    for (int i = 0; i < 3; i++) {
      l_str_out[i] = '.';
    }
  } else {
    strncpy(l_str_out, p_str_in, l_max_head_size);
  }

  // werase(l_loc_header);
  wattron(l_loc_header, COLOR_PAIR(1) | A_BOLD);

  mvwprintw(l_loc_header, 0, (l_col - strlen(l_str_out) - 1) / 2, " %s ",
            l_str_out);

  wrefresh(l_loc_header);

  // ChWnwLook(0, COLOR_PAIR(1) | A_BOLD);

  free(l_str_out);
}

void ChDirList(int p_num, int p_count, struct dirent ***p_list,
               char *p_cur_dir) {

  WINDOW *l_window = g_windows_wnd[p_num];
  g_list_el_count = p_count;
  g_dir_lists[p_num] = *p_list;

  werase(l_window);
  wattron(l_window, COLOR_PAIR(1) | A_BOLD);
  box(l_window, ACS_VLINE, ACS_HLINE);

  // refresh();

  wrefresh(l_window);
}

int MenuManager() {
  int l_row, l_col, l_cur_menu_id, l_input, l_dp_count = g_list_el_count,
                                            l_need_reset;
  MENU *l_my_menu = NULL;
  ITEM **my_item = NULL;

  while (l_input != 'q') {
    l_cur_menu_id = 0, l_input = 0, l_dp_count = g_list_el_count,
    l_need_reset = 0;

    getmaxyx(g_windows_wnd[0], l_row, l_col);

    my_item = (ITEM **)calloc(l_dp_count, sizeof(ITEM *));

    char **full_item_str = calloc(l_dp_count, sizeof(char *));

    for (int i = 1; i < l_dp_count; i++) {
      full_item_str[i] =
          calloc(strlen(g_dir_lists[0][i]->d_name) + 2, sizeof(char));

      full_item_str[i][0] = g_dir_lists[0][i]->d_type == DT_DIR ? '/' : ' ';
      strcat(full_item_str[i], g_dir_lists[0][i]->d_name);

      my_item[i - 1] = new_item(full_item_str[i], "");
    }
    // my_item[l_dp_count] = NULL;

    l_my_menu = new_menu((ITEM **)my_item);
    set_menu_format(l_my_menu, l_row - 2, 1);
    set_menu_win(l_my_menu, g_windows_wnd[0]);
    set_menu_sub(l_my_menu,
                 derwin(g_windows_wnd[0], l_row - 1, l_col - 1, 1, 1));
    set_menu_mark(l_my_menu, "");
    post_menu(l_my_menu);

    while (l_need_reset != 1) {
      wrefresh(g_windows_wnd[0]);
      l_input = getch();
      switch (l_input) {
      case KEY_DOWN:
        menu_driver(l_my_menu, REQ_DOWN_ITEM);
        if (l_cur_menu_id < g_list_el_count - 2)
          l_cur_menu_id++;
        break;
      case KEY_UP:
        menu_driver(l_my_menu, REQ_UP_ITEM);
        if (l_cur_menu_id > 0)
          l_cur_menu_id--;
        break;

      case 'e':
        if (g_dir_lists[0][l_cur_menu_id + 1]->d_type == DT_DIR) {
          EnterDir(g_dir_lists[0][l_cur_menu_id + 1]->d_name);
          l_need_reset = 1;
        }
        break;

      case 'q':
        endwin();
        exit(EXIT_SUCCESS);
        break;
      }
    }

    for (int i = 0; i < l_dp_count; i++) {
      free(full_item_str[i]);
    }
    free(full_item_str);
    for (int i = 0; i < l_dp_count - 1; i++) {

      free_item(my_item[i]);
    }
  }

  // free_menu(my_menu);

  // free(full_item_str);

  // if (l_end != 1) {
  //   getchar();
  //   endwin();
  //   exit(EXIT_SUCCESS);
  // } else {
  //   return l_end;
  // }

  return 0;
}

void GraphicShow() {
  initscr();
  signal(SIGWINCH, SigWinCh);
  cbreak();
  noecho();

  curs_set(FALSE);

  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);

  keypad(stdscr, TRUE);

  refresh();
  InitWnds();
}
