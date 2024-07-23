#include "./10_4_data_get.h"
#include "./10_4_graphic.h"
#include <curses.h>

void CommanderControl() {
  GraphicShow();
  PrintDir();
  MenuManager();
}