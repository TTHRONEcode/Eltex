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

const char *k_g_title_str = "TotalBlack Commander";

struct dirent **g_dir_lists[2];
WINDOW *g_headers_wnd[3], *g_windows_wnd[2], *g_menu_wnd[2];
int g_list_el_count[2], g_cursor_pos[2], g_head_n = 0;

int g_row[2], g_col[2], g_cur_menu_id[2], g_input[2], g_dp_count[2],
    g_need_reset[2], g_max_vis_item[2], g_max_item[2], g_cur_vis_menu_id[2],
    g_min_vis_item[2];
MENU *g_my_menu[2];
ITEM **g_item[2], *g_item_need[2];
char **l_item_str_w_type[2];
char *g_str_out[2];

void ChHeaderDirStr(int p_num, char *p_str_in, int p_need_change) {
  WINDOW *l_loc_header = g_headers_wnd[1 + p_num];
  if (p_need_change == 1) {
    int l_col = 0;
    l_col = getmaxx(l_loc_header);
    int l_max_head_size = l_col > 6 ? l_col - 6 : l_col;
    // int l_max_vis_head_size = (l_max_head_size * 90 / 100);
    int l_dir_str_size = strlen(p_str_in);
    unsigned long l_i = 0;

    free(g_str_out[p_num]);
    g_str_out[p_num] = (char *)calloc((l_max_head_size + 1), sizeof(char));

    if (l_dir_str_size > l_max_head_size) {
      strncpy(g_str_out[p_num], p_str_in + (l_dir_str_size - l_max_head_size),
              l_max_head_size);

      l_i = strlen(g_str_out[p_num]);
      l_i = l_i < 4 ? l_i : 3;

      for (int i = 0; i < l_i; i++) {
        g_str_out[p_num][i] = '.';
      }
    } else {
      strncpy(g_str_out[p_num], p_str_in, l_max_head_size);
    }
  }

  // werase(l_loc_header);
  // if (p_need_change == 1)
  wrefresh(g_windows_wnd[p_num]);

  wattron(l_loc_header, COLOR_PAIR(1));

  mvwprintw(l_loc_header, 0, 2, " %s ", g_str_out[p_num]);

  wrefresh(l_loc_header);

  // ChWnwLook(0, COLOR_PAIR(1) | A_BOLD);
}

static void InitWnds() {
  int row, col;
  WINDOW *case_wnd = NULL;

  // Center
  getmaxyx(stdscr, row, col);

  case_wnd = newwin(1, col, 0, 0);
  wattron(case_wnd, COLOR_PAIR(1));
  mvwprintw(case_wnd, 0, (col - strlen(k_g_title_str) - 2) / 2, " %s ",
            k_g_title_str);
  g_headers_wnd[0] = case_wnd;
  ////////

  // Left Block
  // getmaxyx(g_headers_wnd[0], row, col);
  case_wnd = newwin(row - 1, (col) / 2 + 1, 1, 0);
  wattron(case_wnd, COLOR_PAIR(1));
  box(case_wnd, ACS_VLINE, ACS_HLINE);
  g_windows_wnd[0] = case_wnd;
  // left header
  // getmaxyx(g_windows_wnd[0], row, col);
  case_wnd = newwin(1, (col) / 2 + 1, 1, 0);
  wattron(case_wnd, COLOR_PAIR(1) | A_REVERSE);
  // mvwprintw(case_wnd, 0, (col - strlen(g_cur_dir_left) - 1) / 2 - 1, " %s ",
  //           g_cur_dir_left);
  g_headers_wnd[1] = case_wnd;
  ////////

  // Right
  // getmaxyx(g_headers_wnd[0], row, col);
  case_wnd = newwin(row - 1, (col + 1) / 2, 1, (col) / 2);
  wattron(case_wnd, COLOR_PAIR(1));
  box(case_wnd, ACS_VLINE, ACS_HLINE);
  g_windows_wnd[1] = case_wnd;
  // right header
  // getmaxyx(g_windows_wnd[1], row, col);
  case_wnd = newwin(1, (col + 1) / 2, 1, (col) / 2);
  wattron(case_wnd, COLOR_PAIR(1));
  // mvwprintw(case_wnd, 0, (col - strlen(g_cur_dir_left) - 1) / 2 - 1, " %s ",
  //           g_cur_dir_left);
  g_headers_wnd[2] = case_wnd;
  ////////

  for (int i = 0; i < 2; i++) {
    wrefresh(g_windows_wnd[i]);
    ChHeaderDirStr(i, "super good", 1);
  }
  for (int i = 0; i < 3; i++) {
    wrefresh(g_headers_wnd[i]);
  }

  for (int i = 0; i < 2; i++) {
    g_menu_wnd[i] = derwin(g_windows_wnd[i], 1, 1, 1, 1);
  }

  // for (int i = 0; i < 2; i++) {
  //   g_str_out[i] = (char *)calloc(0, sizeof(char));
  // }
}

static void ChVisMenuId(int k) {
  getmaxyx(g_windows_wnd[k], g_row[k], g_col[k]);

  g_max_item[k] = item_count(g_my_menu[k]);

  g_max_vis_item[k] = g_row[k] - 2;
  if (g_max_vis_item[k] > g_max_item[k])
    g_max_vis_item[k] = g_max_item[k];

  if (g_cur_menu_id[k] < g_max_vis_item[k])
    g_cur_vis_menu_id[k] = g_cur_menu_id[k];
  else {
    if (g_cur_menu_id[k] > (g_max_item[k] - g_max_vis_item[k])) {
      g_cur_vis_menu_id[k] =
          g_max_vis_item[k] - (g_max_item[k] - g_cur_menu_id[k]);
    } else {
      g_cur_vis_menu_id[k] = 0;
    }

    g_min_vis_item[k] = g_cur_menu_id[k] - g_cur_vis_menu_id[k];
  }
}

static void ColorMenuItems(int k, int p_max_vis_item, int p_cur_vis_menu_id,
                           int p_min_vis_item, int p_col, chtype p_dim_attr) {
  for (int i = 0; i < p_max_vis_item; i++) {
    if (i != p_cur_vis_menu_id) {
      if (g_dir_lists[k][i + 1 + p_min_vis_item]->d_type == DT_DIR) {
        mvwchgat(g_windows_wnd[k], i + 1, 1, p_col - 2, A_BOLD | p_dim_attr, 2,
                 NULL);
      } else {
        mvwchgat(g_windows_wnd[k], i + 1, 1, p_col - 2, A_NORMAL | p_dim_attr,
                 1, NULL);
      }
    } else {

      if (g_dir_lists[k][i + 1 + p_min_vis_item]->d_type == DT_DIR) {
        mvwchgat(g_windows_wnd[k], i + 1, 1, p_col - 2,
                 A_BOLD | A_REVERSE | p_dim_attr, 2, NULL);
      } else {
        mvwchgat(g_windows_wnd[k], i + 1, 1, p_col - 2, A_REVERSE | p_dim_attr,
                 1, NULL);
      }
    }
  }

  wrefresh(g_windows_wnd[k]);

  // werase(g_headers_wnd[1 + k]);

  // redrawwin(g_headers_wnd[1 + k]);
  wrefresh(g_headers_wnd[1 + k]);

  // doupdate();
}

static void RedrawMenu(int n) {
  // if (g_item[n] != NULL) {
  // for (int i = 0; i < g_dp_count[n] - 1; i++) {
  //   free_item(g_item[n][i]);
  // }
  // free(g_item[n]);
  // }

  g_dp_count[n] = g_list_el_count[n], g_input[n] = 0;

  g_cur_menu_id[n] = g_cursor_pos[n] - 1, g_max_vis_item[n] = 0,
  g_min_vis_item[n] = 0;

  getmaxyx(g_windows_wnd[n], g_row[n], g_col[n]);

  // free(l_item_str_w_type[n]);
  g_item[n] = (ITEM **)calloc(g_dp_count[n], sizeof(ITEM *));
  l_item_str_w_type[n] = (char **)calloc(g_dp_count[n], sizeof(char *));

  for (int i = 1; i < g_dp_count[n]; i++) {
    l_item_str_w_type[n][i] =
        (char *)calloc(strlen(g_dir_lists[n][i]->d_name) + 2, sizeof(char));

    l_item_str_w_type[n][i][0] =
        g_dir_lists[n][i]->d_type == DT_DIR ? '/' : ' ';
    strcat(l_item_str_w_type[n][i], g_dir_lists[n][i]->d_name);
    g_item[n][i - 1] = new_item(l_item_str_w_type[n][i], "");
  }

  g_my_menu[n] = new_menu((ITEM **)g_item[n]);
  set_menu_format(g_my_menu[n], g_row[n] - 2, 1);
  // set_menu_win(g_my_menu[k], g_windows_wnd[k]);

  wresize(g_menu_wnd[n], g_row[n] - 2, g_col[n] - 2);
  set_menu_sub(g_my_menu[n], g_menu_wnd[n]);
  set_menu_mark(g_my_menu[n], "");

  // wrefresh(g_menu_wnd[k]);
  post_menu(g_my_menu[n]);

  set_current_item(g_my_menu[n], g_item[n][g_cur_menu_id[n]]);

  ChVisMenuId(n);
}
void RedrawBox(int p_num) {
  WINDOW *l_window = g_windows_wnd[p_num];
  werase(l_window);
  wattron(l_window, COLOR_PAIR(1));
  box(l_window, ACS_VLINE, ACS_HLINE);
}
void SigWinCh(int p_signo) {
  struct winsize l_size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&l_size);
  resizeterm(l_size.ws_row, l_size.ws_col);
  // }
  // static void ResizeWindows() {
  int l_row, l_col, l_std_row, l_std_col;
  WINDOW *l_loc_header = NULL;
  WINDOW *l_loc_window = NULL;

  refresh();

  getmaxyx(stdscr, l_row, l_col);
  getmaxyx(stdscr, l_std_row, l_std_col);

  l_loc_header = g_headers_wnd[0];
  werase(l_loc_header);
  if (l_std_row >= 5) {

    mvwin(l_loc_header, 0, 0);
    wresize(l_loc_header, 1, l_std_col);
    wattron(l_loc_header, COLOR_PAIR(1));
    mvwprintw(l_loc_header, 0, (l_std_col - strlen(k_g_title_str) - 2) / 2,
              " %s ", k_g_title_str);
  }
  wrefresh(l_loc_header);

  // left
  l_loc_window = g_windows_wnd[0];
  werase(l_loc_window);
  mvwin(l_loc_window, (l_std_row >= 5 ? 1 : 0), 0);
  wresize(l_loc_window, l_std_row - (l_std_row >= 5 ? 1 : 0),
          (l_std_col) / 2 + 1);
  box(l_loc_window, ACS_VLINE, ACS_HLINE);
  wattron(l_loc_window, COLOR_PAIR(1));
  // wrefresh(l_loc_window);

  // getmaxyx(stdscr, l_std_row, l_std_col);
  l_loc_header = g_headers_wnd[1];
  werase(l_loc_header);
  mvwin(l_loc_header, 1, 0);
  wresize(l_loc_header, 1, (l_std_col) / 2 + 1);
  // wrefresh(l_loc_header);

  //////// right
  l_loc_window = g_windows_wnd[1];
  werase(l_loc_window);
  mvwin(l_loc_window, (l_std_row >= 5 ? 1 : 0), (l_std_col) / 2);
  wresize(l_loc_window, l_std_row - (l_std_row >= 5 ? 1 : 0),
          (l_std_col + 1) / 2);
  box(l_loc_window, ACS_VLINE, ACS_HLINE);
  wattron(l_loc_window, COLOR_PAIR(1));
  // wrefresh(l_loc_window);

  // getmaxyx(stdscr, l_std_row, l_std_col);
  l_loc_header = g_headers_wnd[2];
  werase(l_loc_header);
  mvwin(l_loc_header, (l_std_row >= 5 ? 1 : 0), (l_std_col) / 2);
  wresize(l_loc_header, 1, (l_std_col + 1) / 2);
  // wrefresh(l_loc_header);

  //////////////

  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < g_dp_count[j]; i++) {
      free(l_item_str_w_type[j][i]);
    }
    free(l_item_str_w_type[j]);
  }
  for (int i = 0; i < 2; i++) {
    RedrawBox(i);
    RedrawMenu(i);
    ColorMenuItems(i, g_max_vis_item[i], g_cur_vis_menu_id[i],
                   g_min_vis_item[i], g_col[i], A_NORMAL);
  }

  for (int i = 0; i < 2; i++) {

    redrawwin(g_windows_wnd[i]);
    wrefresh(g_windows_wnd[i]);
    ChHeaderDirStr(i, g_str_out[i], 0);
  }

  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < g_dp_count[j]; i++) {
      free(l_item_str_w_type[j][i]);
    }
    free(l_item_str_w_type[j]);

    free_menu(g_my_menu[j]);
    if (g_dp_count[j] > 0) {
      for (int i = 1; i < g_dp_count[j]; i++) {
        free_item(g_item[j][i - 1]);
      }
    }
    free(g_item[j]);
  }

  // refresh();
}

void ChDirList(int p_num, int p_count, struct dirent ***p_list, char *p_cur_dir,
               int p_cursor_pos) {

  WINDOW *l_window = g_windows_wnd[p_num];
  g_list_el_count[p_num] = p_count;
  g_dir_lists[p_num] = *p_list;
  g_cursor_pos[p_num] = p_cursor_pos;
  RedrawBox(p_num);
  // refresh();

  // wrefresh(l_window);
}

int MenuManager() {

  for (int j = 0; j < 2; j++) {
    RedrawMenu(j);

    // wrefresh(g_windows_wnd[j]);
    g_need_reset[j] = 2;

    ColorMenuItems(j, g_max_vis_item[j], g_cur_vis_menu_id[j],
                   g_min_vis_item[j], g_col[j], A_NORMAL);
  }

  while (1) {

    if (g_need_reset[g_head_n] != 2) {
      RedrawMenu(g_head_n);
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
        for (int i = 0; i < g_dp_count[g_head_n]; i++) {
          free(l_item_str_w_type[g_head_n][i]);
        }
        free(l_item_str_w_type[g_head_n]);

        if (g_dir_lists[g_head_n][g_cur_menu_id[g_head_n] + 1]->d_type ==
            DT_DIR) {
          EnterDir(g_dir_lists[g_head_n][g_cur_menu_id[g_head_n] + 1]->d_name,
                   g_cur_menu_id[g_head_n] + 1, g_head_n);
          g_need_reset[g_head_n] = 1;

          free_menu(g_my_menu[g_head_n]);
          if (g_dp_count[g_head_n] > 0) {
            for (int i = 1; i < g_dp_count[g_head_n]; i++) {
              free_item(g_item[g_head_n][i - 1]);
            }
          }
          free(g_item[g_head_n]);
          // for (int i = 0; i < g_dp_count[g_head_n] - 1; i++) {

          //   free_item(g_item[g_head_n][i]);
          // }
          // free(g_item[g_head_n]);

          // free_item(*g_item[g_head_n]);
        }
        break;
      case '\n':
        for (int i = 0; i < g_dp_count[g_head_n]; i++) {
          free(l_item_str_w_type[g_head_n][i]);
        }
        free(l_item_str_w_type[g_head_n]);

        if (g_dir_lists[g_head_n][g_cur_menu_id[g_head_n] + 1]->d_type ==
            DT_DIR) {
          EnterDir(g_dir_lists[g_head_n][g_cur_menu_id[g_head_n] + 1]->d_name,
                   g_cur_menu_id[g_head_n] + 1, g_head_n);
          g_need_reset[g_head_n] = 1;

          free_menu(g_my_menu[g_head_n]);
          if (g_dp_count[g_head_n] > 0) {
            for (int i = 1; i < g_dp_count[g_head_n]; i++) {
              free_item(g_item[g_head_n][i - 1]);
            }
          }
          free(g_item[g_head_n]);
          // for (int i = 0; i < g_dp_count[g_head_n] - 1; i++) {

          //   free_item(g_item[g_head_n][i]);
          // }
          // free(g_item[g_head_n]);
          // free_item(*g_item[g_head_n]);
        }
        break;

      case 'q':
        for (int i = 0; i < g_dp_count[g_head_n]; i++) {
          free(l_item_str_w_type[g_head_n][i]);
        }
        free(l_item_str_w_type[g_head_n]);
        EnterDir(g_dir_lists[g_head_n][1]->d_name, 1, g_head_n);
        g_need_reset[g_head_n] = 1;

        free_menu(g_my_menu[g_head_n]);
        if (g_dp_count[g_head_n] > 0) {
          for (int i = 1; i < g_dp_count[g_head_n]; i++) {
            free_item(g_item[g_head_n][i - 1]);
          }
        }
        free(g_item[g_head_n]);
        break;

        // case KEY_RESIZE:
        //   EnterDir(g_dir_lists[g_head_n][1]->d_name, 1, g_head_n);
        //   g_need_reset[g_head_n] = 1;
        //   break;

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
        keypad(stdscr, FALSE);

        free_menu(g_my_menu[0]);
        free_menu(g_my_menu[1]);
        for (int j = 0; j < 2; j++) {
          for (int i = 0; i < g_list_el_count[j]; i++) {
            free(l_item_str_w_type[j][i]);
          }
          free(l_item_str_w_type[j]);
          for (int i = 0; i < g_list_el_count[j] - 1; i++) {

            free_item(g_item[j][i]);
          }
          free(g_item[j]);
        }

        for (int i = 0; i < 2; i++) {
          delwin(g_windows_wnd[i]);
        }
        for (int i = 0; i < 3; i++) {
          delwin(g_headers_wnd[i]);
        }

        FreeAllData();

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
  // cbreak();
  noecho();

  curs_set(FALSE);

  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_BLUE, COLOR_BLACK);

  keypad(stdscr, TRUE);
  // keypad(g_windows_wnd[1], TRUE);

  refresh();
  InitWnds();
  // SigWinCh(SIGWINCH);
}
