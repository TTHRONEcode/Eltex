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
#include <time.h>

struct dirent **g_dir_lists[2];

WINDOW *g_headers_wnd[3], *g_windows_wnd[2];
const char *k_g_header_main_str = "TotalBlack Commander";
char *g_cur_dir_left = "aaaaaaaaa"; // TODO remove
int g_list_el_count, g_cursor_pos, g_active_head_num;

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
  mvwprintw(case_wnd, 0, (col - strlen(k_g_header_main_str) - 2) / 2, " %s ",
            k_g_header_main_str);
  g_headers_wnd[0] = case_wnd;
  ////////

  // Left Block
  getmaxyx(g_headers_wnd[0], row, col);
  case_wnd = derwin(g_headers_wnd[0], row - 1, (col) / 2 + 1, 1, 0);
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
  case_wnd = derwin(g_headers_wnd[0], row - 1, (col + 1) / 2, 1, (col) / 2);
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

  mvwprintw(l_loc_header, 0, (l_col - strlen(l_str_out)) / 2 - 1, " %s ",
            l_str_out);

  // wrefresh(l_loc_header);

  // ChWnwLook(0, COLOR_PAIR(1) | A_BOLD);

  free(l_str_out);
}

void ChDirList(int p_num, int p_count, struct dirent ***p_list, char *p_cur_dir,
               int p_cursor_pos) {

  WINDOW *l_window = g_windows_wnd[p_num];
  g_list_el_count = p_count;
  g_dir_lists[p_num] = *p_list;
  g_cursor_pos = p_cursor_pos;

  werase(l_window);
  wattron(l_window, COLOR_PAIR(1) | A_BOLD);
  box(l_window, ACS_VLINE, ACS_HLINE);

  // refresh();

  // wrefresh(l_window);
}

void ColorMenuItems(int p_max_vis_item, int p_cur_vis_menu_id,
                    int p_min_vis_item, int p_col) {
  for (int i = 0; i < p_max_vis_item; i++) {
    if (i != p_cur_vis_menu_id) {
      if (g_dir_lists[0][i + 1 + p_min_vis_item]->d_type == DT_DIR) {
        mvwchgat(g_windows_wnd[0], i + 1, 1, p_col - 2, A_BOLD, 2, NULL);
      } else {
        mvwchgat(g_windows_wnd[0], i + 1, 1, p_col - 2, A_NORMAL, 1, NULL);
      }
    } else {

      if (g_dir_lists[0][i + 1 + p_min_vis_item]->d_type == DT_DIR) {
        mvwchgat(g_windows_wnd[0], i + 1, 1, p_col - 2, A_BOLD | A_REVERSE, 2,
                 NULL);
      } else {
        mvwchgat(g_windows_wnd[0], i + 1, 1, p_col - 2, A_REVERSE, 1, NULL);
      }
    }
  }
}

int MenuManager() {
  int l_row, l_col, l_cur_menu_id, l_input,
      l_dp_count = g_list_el_count, l_need_reset, l_max_vis_item, l_max_item,
      l_cur_vis_menu_id, l_min_vis_item;
  MENU *l_my_menu = NULL;
  ITEM **l_item = NULL, *l_item_need = NULL;

  while (l_input != 'q') {
    l_cur_menu_id = g_cursor_pos - 1, l_dp_count = g_list_el_count, l_input = 0,
    l_need_reset = 0, l_max_vis_item = 0, l_min_vis_item = 0;

    getmaxyx(g_windows_wnd[0], l_row, l_col);

    l_item = (ITEM **)calloc(l_dp_count, sizeof(ITEM *));
    char **l_item_str_w_type = calloc(l_dp_count, sizeof(char *));

    for (int i = 1; i < l_dp_count; i++) {
      l_item_str_w_type[i] =
          calloc(strlen(g_dir_lists[0][i]->d_name) + 2, sizeof(char));

      l_item_str_w_type[i][0] = g_dir_lists[0][i]->d_type == DT_DIR ? '/' : ' ';
      strcat(l_item_str_w_type[i], g_dir_lists[0][i]->d_name);
      l_item[i - 1] = new_item(l_item_str_w_type[i], "");
    }

    l_my_menu = new_menu((ITEM **)l_item);
    set_menu_format(l_my_menu, l_row - 2, 1);
    set_menu_win(l_my_menu, g_windows_wnd[0]);
    set_menu_sub(l_my_menu,
                 derwin(g_windows_wnd[0], l_row - 1, l_col - 2, 1, 1));
    set_menu_mark(l_my_menu, "");
    post_menu(l_my_menu);

    set_current_item(l_my_menu, l_item[l_cur_menu_id]);

    l_max_item = item_count(l_my_menu);

    l_max_vis_item = l_row - 2;
    if (l_max_vis_item > l_max_item)
      l_max_vis_item = l_max_item;

    if (l_cur_menu_id < l_max_vis_item)
      l_cur_vis_menu_id = l_cur_menu_id;
    else {
      if (l_cur_menu_id > (l_max_item - l_max_vis_item)) {
        l_cur_vis_menu_id = l_max_vis_item - (l_max_item - l_cur_menu_id);
      } else {
        l_cur_vis_menu_id = 0;
      }

      l_min_vis_item = l_cur_vis_menu_id + l_cur_menu_id;
    }

    while (l_need_reset != 1) {
      ColorMenuItems(l_max_vis_item, l_cur_vis_menu_id, l_min_vis_item, l_col);

      wrefresh(g_windows_wnd[0]);

      l_input = getch();

      switch (l_input) {
      case KEY_DOWN:
        // wchgat(g_windows_wnd[0], l_col - 2, A_NORMAL, 1, NULL);
        menu_driver(l_my_menu, REQ_DOWN_ITEM);
        if (l_cur_menu_id < l_dp_count - 2) {
          l_cur_menu_id++;

          if (l_cur_menu_id < l_max_vis_item) {
            l_cur_vis_menu_id++;
          } else {
            if (l_cur_vis_menu_id + 1 < l_max_vis_item)
              l_cur_vis_menu_id++;
            else
              l_min_vis_item++;
          }
        }

        break;
      case KEY_UP:
        // wchgat(g_windows_wnd[0], l_col - 2, A_NORMAL, 1, NULL);
        menu_driver(l_my_menu, REQ_UP_ITEM);
        if (l_cur_menu_id > 0) {
          l_cur_menu_id--;

          if (l_cur_menu_id >= l_min_vis_item) {
            l_cur_vis_menu_id--;
          } else {
            if (l_cur_menu_id - 1 >= l_min_vis_item)
              l_cur_vis_menu_id--;
            else
              l_min_vis_item--;
          }
        }

        break;

      case 'e':
        if (g_dir_lists[0][l_cur_menu_id + 1]->d_type == DT_DIR) {
          EnterDir(g_dir_lists[0][l_cur_menu_id + 1]->d_name,
                   l_cur_menu_id + 1);
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
      free(l_item_str_w_type[i]);
    }
    free(l_item_str_w_type);
    for (int i = 0; i < l_dp_count - 1; i++) {

      free_item(l_item[i]);
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
  init_pair(2, COLOR_BLUE, COLOR_BLACK);

  keypad(stdscr, TRUE);

  refresh();
  InitWnds();
}
