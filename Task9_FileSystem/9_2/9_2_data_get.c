#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./9_2_graphic.h"

char g_cur_dir_name[80000] = {0}; // TODO заменить это нечто на malloc
struct dirent **g_dirents;
int g_count;

static int TypeAlphaSort(const struct dirent **p_e_1,
                         const struct dirent **p_e_2) {
  int l_c = 0;

  if ((*p_e_1)->d_type == DT_DIR) {
    if ((*p_e_2)->d_type == DT_DIR)
      l_c = strcoll((*p_e_1)->d_name, (*p_e_2)->d_name);
    else
      l_c = -1;

  } else {
    if ((*p_e_2)->d_type != DT_DIR)
      l_c = strcoll((*p_e_1)->d_name, (*p_e_2)->d_name);
    else
      l_c = 1;
  }

  return l_c;
}

static void FindAbsDir() { // TODO сделать обработку ВСЕХ ошибок
  ino_t l_this_dir_inode[20] = {0};
  char l_loc_name[10000] = {0};
  int l_inode_num = -1;

  strcat(l_loc_name, ".");
  g_count = scandir(l_loc_name, &g_dirents, NULL, TypeAlphaSort);

  do {
    l_inode_num++;
    l_this_dir_inode[l_inode_num] = g_dirents[0]->d_ino;

    strcat(l_loc_name, "/..");

    g_count = scandir(l_loc_name, &g_dirents, NULL, TypeAlphaSort);

  } while (l_this_dir_inode[l_inode_num] != g_dirents[0]->d_ino);

  g_cur_dir_name[0] = '/';
  for (int i = l_inode_num - 1; i >= 0; i--) {
    g_count = scandir(g_cur_dir_name, &g_dirents, NULL, TypeAlphaSort);
    for (int j = 0; j < g_count; j++) {
      if (l_this_dir_inode[i] == g_dirents[j]->d_ino) {
        strcat(g_cur_dir_name, g_dirents[j]->d_name);
        strcat(g_cur_dir_name, "/");

        break;
      }
    }
  }

  g_cur_dir_name[strlen(g_cur_dir_name)] = 0;
  g_count = scandir(g_cur_dir_name, &g_dirents, NULL, TypeAlphaSort);
}

void PrintDir() {
  // FindAbsDir();
  g_cur_dir_name[0] = '/';
  g_count = scandir(g_cur_dir_name, &g_dirents, NULL, TypeAlphaSort);

  ChDirList(0, g_count, &g_dirents, g_cur_dir_name);
  ChHeaderDirStr(0, g_cur_dir_name);

  // free(dp); // TODO
}

void EnterDir(char *p_dir) {
  strcat(g_cur_dir_name, "/");
  strcat(g_cur_dir_name, p_dir);

  g_count = scandir(g_cur_dir_name, &g_dirents, NULL, TypeAlphaSort);
  ChDirList(0, g_count, &g_dirents, g_cur_dir_name);
  ChHeaderDirStr(0, g_cur_dir_name);
}
