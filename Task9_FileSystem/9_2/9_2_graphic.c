#include <dirent.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>

WINDOW *headers_wnd[3], *windows_wnd[2];
char *header_main_str = "TotalBlack Commander";
char *cur_dir_left = ".../home/Doc/";

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
  case_wnd = derwin(headers_wnd[0], row - 1, (col - 1) / 2, 1, 1);
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
  case_wnd = derwin(headers_wnd[0], row - 1, (col - 1) / 2, 1, col / 2);
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
  // int col = getmaxx(loc_header);
  //  wattron(loc_header, COLOR_PAIR(1) | A_BOLD);
  //  mvwprintw(loc_header, 0, (col - strlen(ptr_head) - 1) / 2 - 1, " %s ",
  //            ptr_head);

  mvwprintw(loc_header, 0, 1, " %s ", ptr_head);

  refresh();
  wrefresh(loc_header);
}

void ChList(int num, int count, struct dirent **list, char *cur_dir) {
  WINDOW *loc_window = windows_wnd[num];
  int row = getmaxx(loc_window);

  for (int i = 1; (i < row - 1 && i < count); i++) {
    mvwprintw(loc_window, i, 1, "%c%s\n",
              (list[i]->d_type == DT_DIR ? '/' : ' '), (list[i]->d_name));
  }

  box(loc_window, ACS_VLINE, ACS_HLINE);
  refresh();
  wrefresh(loc_window);
  ChHeader(num, cur_dir);

  char a = 'a';
  while ((a = getch()) != 'q')
    ;
  endwin();
  exit(EXIT_SUCCESS);
}

void GraphicShow() {
  initscr();
  signal(SIGWINCH, SigWinCh);
  cbreak();

  curs_set(FALSE);
  start_color();

  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);

  refresh();
  InitWnds();

  ChHeader(0, "~/ubun/Desktop");

  // refresh();

  // header_l_wnd = derwin(stdscr, 1, col, 0, 0);
  // wattron(header_l_wnd, COLOR_PAIR(2) | A_BOLD);

  // subwnd = derwin(wnd, 3, 16, 1, 1);
  // wbkgd(subwnd, COLOR_PAIR(2));

  // wattron(subwnd, A_BOLD);
  // wprintw(subwnd, "%d %d", row, col);

  // wrefresh(subwnd);
  // wrefresh(wnd);

  // delwin(subwnd);
  // delwin(wnd);

  // wmove(stdscr, 8, 1);
  // printw("Press any key to continue...");
  // refresh();
}
