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

const char *k_g_header_main_str = "TotalBlack Commander";

struct dirent **g_dir_lists[2];
WINDOW *g_headers_wnd[3], *g_windows_wnd[2];

char *g_cur_dir_left = "aaaaaaaaa"; // TODO remove
int g_list_el_count[2], g_cursor_pos[2], g_head_n = 0;

int g_row[2], g_col[2], g_cur_menu_id[2], g_input[2], g_dp_count[2],
    g_need_reset[2], g_max_vis_item[2], g_max_item[2], g_cur_vis_menu_id[2],
    g_min_vis_item[2];
MENU *g_my_menu[2];
ITEM **g_item[2], *g_item_need[2];
char **l_item_str_w_type[2];

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

  wrefresh(l_loc_header);

  // ChWnwLook(0, COLOR_PAIR(1) | A_BOLD);

  free(l_str_out);
}

void ChDirList(int p_num, int p_count, struct dirent ***p_list, char *p_cur_dir,
               int p_cursor_pos) {

  WINDOW *l_window = g_windows_wnd[p_num];
  g_list_el_count[p_num] = p_count;
  g_dir_lists[p_num] = *p_list;
  g_cursor_pos[p_num] = p_cursor_pos;

  werase(l_window);
  wattron(l_window, COLOR_PAIR(1) | A_BOLD);
  box(l_window, ACS_VLINE, ACS_HLINE);

  // refresh();

  // wrefresh(l_window);
}

static void ColorMenuItems(int p_head_n, int p_max_vis_item,
                           int p_cur_vis_menu_id, int p_min_vis_item, int p_col,
                           chtype p_dim_attr) {
  for (int i = 0; i < p_max_vis_item; i++) {
    if (i != p_cur_vis_menu_id) {
      if (g_dir_lists[p_head_n][i + 1 + p_min_vis_item]->d_type == DT_DIR) {
        mvwchgat(g_windows_wnd[p_head_n], i + 1, 1, p_col - 2,
                 A_BOLD | p_dim_attr, 2, NULL);
      } else {
        mvwchgat(g_windows_wnd[p_head_n], i + 1, 1, p_col - 2,
                 A_NORMAL | p_dim_attr, 1, NULL);
      }
    } else {

      if (g_dir_lists[p_head_n][i + 1 + p_min_vis_item]->d_type == DT_DIR) {
        mvwchgat(g_windows_wnd[p_head_n], i + 1, 1, p_col - 2,
                 A_BOLD | A_REVERSE | p_dim_attr, 2, NULL);
      } else {
        mvwchgat(g_windows_wnd[p_head_n], i + 1, 1, p_col - 2,
                 A_REVERSE | p_dim_attr, 1, NULL);
      }
    }
  }

  wrefresh(g_windows_wnd[p_head_n]);
}

int MenuManager() {

  for (int j = 0; j < 2; j++) {
    g_dp_count[j] = g_list_el_count[j], g_input[j] = 0;

    g_cur_menu_id[j] = g_cursor_pos[j] - 1, g_max_vis_item[j] = 0,
    g_min_vis_item[j] = 0;

    getmaxyx(g_windows_wnd[j], g_row[j], g_col[j]);

    g_item[j] = (ITEM **)calloc(g_dp_count[j], sizeof(ITEM *));
    l_item_str_w_type[j] = (char **)calloc(g_dp_count[j], sizeof(char *));

    for (int i = 1; i < g_dp_count[j]; i++) {
      l_item_str_w_type[j][i] =
          (char *)calloc(strlen(g_dir_lists[j][i]->d_name) + 2, sizeof(char));

      l_item_str_w_type[j][i][0] =
          g_dir_lists[j][i]->d_type == DT_DIR ? '/' : ' ';
      strcat(l_item_str_w_type[j][i], g_dir_lists[j][i]->d_name);
      g_item[j][i - 1] = new_item(l_item_str_w_type[j][i], "");
    }

    g_my_menu[j] = new_menu((ITEM **)g_item[j]);
    set_menu_format(g_my_menu[j], g_row[j] - 2, 1);
    set_menu_win(g_my_menu[j], g_windows_wnd[j]);
    set_menu_sub(g_my_menu[j],
                 derwin(g_windows_wnd[j], g_row[j] - 1, g_col[j] - 2, 1, 1));
    set_menu_mark(g_my_menu[j], "");
    post_menu(g_my_menu[j]);

    set_current_item(g_my_menu[j], g_item[j][g_cur_menu_id[j]]);

    g_max_item[j] = item_count(g_my_menu[j]);

    g_max_vis_item[j] = g_row[j] - 2;
    if (g_max_vis_item[j] > g_max_item[j])
      g_max_vis_item[j] = g_max_item[j];

    if (g_cur_menu_id[j] < g_max_vis_item[j])
      g_cur_vis_menu_id[j] = g_cur_menu_id[j];
    else {
      if (g_cur_menu_id[j] > (g_max_item[j] - g_max_vis_item[j])) {
        g_cur_vis_menu_id[j] =
            g_max_vis_item[j] - (g_max_item[j] - g_cur_menu_id[j]);
      } else {
        g_cur_vis_menu_id[j] = 0;
      }

      g_min_vis_item[j] = g_cur_menu_id[j] - g_cur_vis_menu_id[j];
    }

    // wrefresh(g_windows_wnd[j]);
    g_need_reset[j] = 2;

    ColorMenuItems(j, g_max_vis_item[j], g_cur_vis_menu_id[j],
                   g_min_vis_item[j], g_col[j], A_NORMAL);
  }

  while (1) {

    if (g_need_reset[g_head_n] != 2) {

      // }

      g_dp_count[g_head_n] = g_list_el_count[g_head_n], g_input[g_head_n] = 0;

      g_cur_menu_id[g_head_n] = g_cursor_pos[g_head_n] - 1,
      g_max_vis_item[g_head_n] = 0, g_min_vis_item[g_head_n] = 0;

      getmaxyx(g_windows_wnd[g_head_n], g_row[g_head_n], g_col[g_head_n]);

      g_item[g_head_n] = (ITEM **)calloc(g_dp_count[g_head_n], sizeof(ITEM *));
      l_item_str_w_type[g_head_n] =
          (char **)calloc(g_dp_count[g_head_n], sizeof(char *));

      for (int i = 1; i < g_dp_count[g_head_n]; i++) {
        l_item_str_w_type[g_head_n][i] = (char *)calloc(
            strlen(g_dir_lists[g_head_n][i]->d_name) + 2, sizeof(char));

        l_item_str_w_type[g_head_n][i][0] =
            g_dir_lists[g_head_n][i]->d_type == DT_DIR ? '/' : ' ';
        strcat(l_item_str_w_type[g_head_n][i],
               g_dir_lists[g_head_n][i]->d_name);
        g_item[g_head_n][i - 1] = new_item(l_item_str_w_type[g_head_n][i], "");
      }

      g_my_menu[g_head_n] = new_menu((ITEM **)g_item[g_head_n]);
      set_menu_format(g_my_menu[g_head_n], g_row[g_head_n] - 2, 1);
      set_menu_win(g_my_menu[g_head_n], g_windows_wnd[g_head_n]);
      set_menu_sub(g_my_menu[g_head_n],
                   derwin(g_windows_wnd[g_head_n], g_row[g_head_n] - 1,
                          g_col[g_head_n] - 2, 1, 1));
      set_menu_mark(g_my_menu[g_head_n], "");
      post_menu(g_my_menu[g_head_n]);

      set_current_item(g_my_menu[g_head_n],
                       g_item[g_head_n][g_cur_menu_id[g_head_n]]);

      g_max_item[g_head_n] = item_count(g_my_menu[g_head_n]);

      g_max_vis_item[g_head_n] = g_row[g_head_n] - 2;
      if (g_max_vis_item[g_head_n] > g_max_item[g_head_n])
        g_max_vis_item[g_head_n] = g_max_item[g_head_n];

      if (g_cur_menu_id[g_head_n] < g_max_vis_item[g_head_n])
        g_cur_vis_menu_id[g_head_n] = g_cur_menu_id[g_head_n];
      else {
        if (g_cur_menu_id[g_head_n] >
            (g_max_item[g_head_n] - g_max_vis_item[g_head_n])) {
          g_cur_vis_menu_id[g_head_n] =
              g_max_vis_item[g_head_n] -
              (g_max_item[g_head_n] - g_cur_menu_id[g_head_n]);
        } else {
          g_cur_vis_menu_id[g_head_n] = 0;
        }

        g_min_vis_item[g_head_n] =
            g_cur_menu_id[g_head_n] - g_cur_vis_menu_id[g_head_n];
      }
    }
    g_need_reset[g_head_n] = 0;

    while (g_need_reset[g_head_n] == 0) {
      ColorMenuItems(g_head_n, g_max_vis_item[g_head_n],
                     g_cur_vis_menu_id[g_head_n], g_min_vis_item[g_head_n],
                     g_col[g_head_n], A_NORMAL);

      g_input[g_head_n] = getch();

      switch (g_input[g_head_n]) {
      case KEY_DOWN:
        // wchgat(g_windows_wnd[0], l_col - 2, A_NORMAL, 1, NULL);
        menu_driver(g_my_menu[g_head_n], REQ_DOWN_ITEM);
        if (g_cur_menu_id[g_head_n] < g_dp_count[g_head_n] - 2) {
          g_cur_menu_id[g_head_n]++;

          if (g_cur_menu_id[g_head_n] < g_max_vis_item[g_head_n]) {
            g_cur_vis_menu_id[g_head_n]++;
          } else {
            if (g_cur_vis_menu_id[g_head_n] + 1 < g_max_vis_item[g_head_n])
              g_cur_vis_menu_id[g_head_n]++;
            else
              g_min_vis_item[g_head_n]++;
          }
        }

        break;

      case KEY_UP:
        // wchgat(g_windows_wnd[0], l_col - 2, A_NORMAL, 1, NULL);
        menu_driver(g_my_menu[g_head_n], REQ_UP_ITEM);
        if (g_cur_menu_id[g_head_n] > 0) {
          g_cur_menu_id[g_head_n]--;

          if (g_cur_menu_id[g_head_n] >= g_min_vis_item[g_head_n]) {
            g_cur_vis_menu_id[g_head_n]--;
          } else {
            if (g_cur_menu_id[g_head_n] - 1 >= g_min_vis_item[g_head_n])
              g_cur_vis_menu_id[g_head_n]--;
            else
              g_min_vis_item[g_head_n]--;
          }
        }

        break;

      case 'e':
        if (g_dir_lists[g_head_n][g_cur_menu_id[g_head_n] + 1]->d_type ==
            DT_DIR) {
          EnterDir(g_dir_lists[g_head_n][g_cur_menu_id[g_head_n] + 1]->d_name,
                   g_cur_menu_id[g_head_n] + 1, g_head_n);
          g_need_reset[g_head_n] = 1;

          for (int i = 0; i < g_dp_count[g_head_n]; i++) {
            free(l_item_str_w_type[g_head_n][i]);
          }
          free(l_item_str_w_type[g_head_n]);
          for (int i = 0; i < g_dp_count[g_head_n] - 1; i++) {

            free_item(g_item[g_head_n][i]);
          }
        }
        break;
      case '\n':
        if (g_dir_lists[g_head_n][g_cur_menu_id[g_head_n] + 1]->d_type ==
            DT_DIR) {
          EnterDir(g_dir_lists[g_head_n][g_cur_menu_id[g_head_n] + 1]->d_name,
                   g_cur_menu_id[g_head_n] + 1, g_head_n);
          g_need_reset[g_head_n] = 1;

          for (int i = 0; i < g_dp_count[g_head_n]; i++) {
            free(l_item_str_w_type[g_head_n][i]);
          }
          free(l_item_str_w_type[g_head_n]);
          for (int i = 0; i < g_dp_count[g_head_n] - 1; i++) {

            free_item(g_item[g_head_n][i]);
          }
        }
        break;

      case 'q':
        EnterDir(g_dir_lists[g_head_n][1]->d_name, 1, g_head_n);
        g_need_reset[g_head_n] = 1;
        break;

      case '\t':
        ColorMenuItems(g_head_n, g_max_vis_item[g_head_n],
                       g_cur_vis_menu_id[g_head_n], g_min_vis_item[g_head_n],
                       g_col[g_head_n], A_DIM);

        // for (int i = 0; i < g_dp_count[g_head_n]; i++) {
        //   free(l_item_str_w_type[g_head_n][i]);
        // }
        // free(l_item_str_w_type[g_head_n]);
        // for (int i = 0; i < g_dp_count[g_head_n] - 1; i++) {

        //   free_item(g_item[g_head_n][i]);
        // }

        g_head_n = 1 - g_head_n;
        g_need_reset[g_head_n] = 2;
        break;

      case KEY_F(10):
        // getchar();
        free_menu(g_my_menu[0]);
        free_menu(g_my_menu[1]);

        for (int i = 0; i < g_dp_count[g_head_n]; i++) {
          free(l_item_str_w_type[g_head_n][i]);
        }
        free(l_item_str_w_type[g_head_n]);
        for (int i = 0; i < g_dp_count[g_head_n] - 1; i++) {

          free_item(g_item[g_head_n][i]);
        }

        endwin();
        exit(EXIT_SUCCESS);
        break;
      }
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
