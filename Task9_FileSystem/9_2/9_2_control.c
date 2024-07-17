#include "./9_2_data_get.h"
#include "./9_2_graphic.h"
#include <curses.h>
// #include <stdio.h>
// #include <stdlib.h>

static int a;

void CommanderControl() {
  GraphicShow();
  PrintDir();
  MenuManager();
}