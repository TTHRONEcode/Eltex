#ifndef INCLUDE_GRAPHIC
#define INCLUDE_GRAPHIC

void GraphicShow();
int MenuManager();

#include <dirent.h>
#include <menu.h>
void ChDirList(int, int, struct dirent ***, char *, int);
void ChHeaderDirStr(int, char *);

#endif
