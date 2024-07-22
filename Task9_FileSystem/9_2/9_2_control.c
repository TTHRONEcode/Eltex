#include "./9_2_data_get.h"
#include "./9_2_graphic.h"
#include <curses.h>

void CommanderControl() {
  GraphicShow();
  PrintDir();
  MenuManager();
}