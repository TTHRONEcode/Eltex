#ifndef INCLUDE_GRAPHIC
#define INCLUDE_GRAPHIC

void GraphicShow();
int MenuManager();

#include <dirent.h>
void ChDirList(int, int, struct dirent ***, char *);
void ChHeaderDirStr(int, char *);

#endif
