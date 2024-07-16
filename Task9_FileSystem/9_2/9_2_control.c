#include "./9_2_data_get.h"
#include "./9_2_graphic.h"

static int a;

void CommanderControl() {
  GraphicShow();
  PrintDir();

  while ((a = MenuManager()) != 0) {
    MenuManager();
  }
}