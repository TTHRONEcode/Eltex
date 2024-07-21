#include "./9_2_data_get.h"
#include "./9_2_graphic.h"
#include <curses.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdlib.h>

void CommanderControl() {
  GraphicShow();
  InitDirsData();
  // getchar();
  // SigWinCh(0);
  MenuManager();

  // getchar();
  // endwin();
  // exit(EXIT_SUCCESS);
}