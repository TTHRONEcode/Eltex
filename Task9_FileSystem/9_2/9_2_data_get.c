#include <dirent.h>
#include <menu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./9_2_graphic.h"

char g_cur_dir_name[2][1000]; // TODO заменить это нечто на malloc
struct dirent **g_dirents[2];
int g_count[2], g_bef_fold_num[2];
char g_bef_fold_name[2][20][20];

static int g_dir_n;

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
  g_count[g_dir_n] =
      scandir(l_loc_name, &g_dirents[g_dir_n], NULL, TypeAlphaSort);

  do {
    l_inode_num++;
    l_this_dir_inode[l_inode_num] = g_dirents[g_dir_n][0]->d_ino;

    strcat(l_loc_name, "/..");

    g_count[g_dir_n] =
        scandir(l_loc_name, &g_dirents[g_dir_n], NULL, TypeAlphaSort);

  } while (l_this_dir_inode[l_inode_num] != g_dirents[g_dir_n][0]->d_ino);

  g_cur_dir_name[g_dir_n][0] = '/';
  for (int i = l_inode_num - 1; i >= 0; i--) {
    g_count[g_dir_n] = scandir(g_cur_dir_name[g_dir_n], &g_dirents[g_dir_n],
                               NULL, TypeAlphaSort);
    for (int j = 0; j < g_count[g_dir_n]; j++) {
      if (l_this_dir_inode[i] == g_dirents[g_dir_n][j]->d_ino) {
        strcat(g_cur_dir_name[g_dir_n], g_dirents[g_dir_n][j]->d_name);
        strcat(g_cur_dir_name[g_dir_n], "/");

        break;
      }
    }
  }

  g_cur_dir_name[g_dir_n][strlen(g_cur_dir_name[g_dir_n]) - 1] = 0;
  g_count[g_dir_n] = scandir(g_cur_dir_name[g_dir_n], &g_dirents[g_dir_n], NULL,
                             TypeAlphaSort);
}

void PrintDir() {
  g_dir_n = 0;

  FindAbsDir();
  // g_cur_dir_name[g_dir_n][0] = '/';
  // g_count[g_dir_n] = scandir(g_cur_dir_name[g_dir_n], &g_dirents[g_dir_n],
  // NULL,
  //                            TypeAlphaSort);

  ChDirList(g_dir_n, g_count[g_dir_n], &g_dirents[g_dir_n],
            g_cur_dir_name[g_dir_n], 1);
  ChHeaderDirStr(g_dir_n, g_cur_dir_name[g_dir_n]);

  g_dir_n = 1;

  // FindAbsDir();
  g_cur_dir_name[g_dir_n][0] = '/';
  g_count[g_dir_n] = scandir(g_cur_dir_name[g_dir_n], &g_dirents[g_dir_n], NULL,
                             TypeAlphaSort);

  ChDirList(g_dir_n, g_count[g_dir_n], &g_dirents[g_dir_n],
            g_cur_dir_name[g_dir_n], 1);

  ChHeaderDirStr(g_dir_n, g_cur_dir_name[g_dir_n]);

  g_dir_n = 0;

  // free(dp); // TODO
}

void EnterDir(char *p_dir, int p_cur_item, int p_act_head) {
  unsigned long l_item_ino = 0;
  int l_need_pos = 1, l_need_name_search = 1;

  if (strcmp(p_dir, "..") == 0) {
    l_item_ino = g_dirents[p_act_head][0]->d_ino;

    int l_strlen = 0;
    while (g_cur_dir_name[p_act_head][(
               l_strlen = strlen(g_cur_dir_name[p_act_head]) - 1)] != '/') {
      g_cur_dir_name[p_act_head][l_strlen] = 0;
    }
    if (l_strlen == 0)
      g_cur_dir_name[p_act_head][0] = '/';
    else
      g_cur_dir_name[p_act_head][l_strlen] = 0;

  } else {
    g_bef_fold_num[p_act_head]++;
    strcpy(g_bef_fold_name[p_act_head][g_bef_fold_num[p_act_head]],
           g_dirents[p_act_head][p_cur_item]->d_name);

    if (g_cur_dir_name[p_act_head][strlen(g_cur_dir_name[p_act_head]) - 1] !=
        '/')
      strcat(g_cur_dir_name[p_act_head], "/");

    strcat(g_cur_dir_name[p_act_head], p_dir);
  }

  g_count[p_act_head] = scandir(g_cur_dir_name[p_act_head],
                                &g_dirents[p_act_head], NULL, TypeAlphaSort);

  for (int i = 1; i < g_count[p_act_head]; i++) {
    if (l_item_ino == g_dirents[p_act_head][i]->d_ino) {
      l_need_pos = i;
      l_need_name_search = 0;
      for (int j = 0;
           j < strlen(g_bef_fold_name[p_act_head][g_bef_fold_num[p_act_head]]);
           j++) {
        g_bef_fold_name[p_act_head][g_bef_fold_num[p_act_head]][j] = 0;
      }
      g_bef_fold_num[p_act_head]--;

      break;
    }
  }
  if (l_need_name_search == 1) {
    for (int i = 0; i < g_count[p_act_head]; i++) {
      if (strcmp(g_bef_fold_name[p_act_head][g_bef_fold_num[p_act_head]],
                 g_dirents[p_act_head][i]->d_name) == 0) {
        g_bef_fold_num[p_act_head]--;
        l_need_pos = i;
        for (int j = 0;
             j <
             strlen(g_bef_fold_name[p_act_head][g_bef_fold_num[p_act_head]]);
             j++) {
          g_bef_fold_name[p_act_head][g_bef_fold_num[p_act_head]][j] = 0;
        }
        break;
      }
    }
  }

  ChDirList(p_act_head, g_count[p_act_head], &g_dirents[p_act_head],
            g_cur_dir_name[p_act_head], l_need_pos);
  ChHeaderDirStr(p_act_head, g_cur_dir_name[p_act_head]);
}
