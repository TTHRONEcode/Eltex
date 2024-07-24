#include <dirent.h>
#include <err.h>
#include <menu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "./10_4_graphic.h" //

static char *g_cur_dir_name[2];
struct dirent **g_dirents[2];
int g_dir_count[2], g_bef_fold_num[2];
char **g_bef_fold_name[2];

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

static void *SafeCalloc(int p_nmemb, size_t p_size, int p_call_line) {
  char *p = calloc(p_nmemb, p_size);
  if (p == NULL) {
    endwin();
    err(EXIT_FAILURE, "calloc on line: %d", p_call_line);
  }

  return p;
}

static int SafeScandir(const char *__restrict __dir,
                       struct dirent ***__restrict __namelist,
                       int (*__selector)(const struct dirent *),
                       int (*__cmp)(const struct dirent **,
                                    const struct dirent **),
                       int __call_line) {
  int l = scandir(__dir, __namelist, __selector, __cmp);
  if (l == -1) {
    endwin();
    err(EXIT_FAILURE, "scandir at line: %d folder: %s", __call_line, __dir);
  }

  return l;
}

void PrintDir() {

  for (int i = 0; i < 2; i++) {
    g_cur_dir_name[i] = realpath(i == 0 ? "." : "/", NULL);

    g_dir_count[i] = SafeScandir(g_cur_dir_name[i], &g_dirents[i], NULL,
                                 TypeAlphaSort, __LINE__);

    ChDirList(i, g_dir_count[i], &g_dirents[i], g_cur_dir_name[i], 1);
    ChHeaderDirStr(i, g_cur_dir_name[i], 1);
  }
}

void EnterDir(char *p_dir, int p_cur_item, int p_head_n) {
  unsigned long l_item_ino = 0;
  int l_need_pos = 1, l_need_name_search = 1;

  if (strcmp(p_dir, "..") == 0) {
    l_item_ino = g_dirents[p_head_n][0]->d_ino;

    int l_strlen = 0;
    while (g_cur_dir_name[p_head_n][(
               l_strlen = strlen(g_cur_dir_name[p_head_n]) - 1)] != '/') {
      g_cur_dir_name[p_head_n][l_strlen] = 0;
    }
    if (l_strlen == 0)
      strcpy(g_cur_dir_name[p_head_n], "/");
    else
      g_cur_dir_name[p_head_n][l_strlen] = 0;

    g_cur_dir_name[p_head_n] =
        (char *)realloc(g_cur_dir_name[p_head_n],
                        (strlen(g_cur_dir_name[p_head_n]) + 1) * sizeof(char));

  } else {
    g_bef_fold_num[p_head_n]++;

    g_bef_fold_name[p_head_n] =
        (char **)realloc(g_bef_fold_name[p_head_n],
                         g_bef_fold_num[p_head_n] * sizeof(char *) * 2);
    g_bef_fold_name[p_head_n][g_bef_fold_num[p_head_n]] =
        SafeCalloc(258, sizeof(char), __LINE__);

    strcpy(g_bef_fold_name[p_head_n][g_bef_fold_num[p_head_n]],
           g_dirents[p_head_n][p_cur_item]->d_name);

    if (g_cur_dir_name[p_head_n][strlen(g_cur_dir_name[p_head_n]) - 1] != '/') {
      g_cur_dir_name[p_head_n] = (char *)realloc(
          g_cur_dir_name[p_head_n],
          (strlen(g_cur_dir_name[p_head_n]) + 2) * sizeof(char));

      strcat(g_cur_dir_name[p_head_n], "/");
    }
    g_cur_dir_name[p_head_n] = (char *)realloc(
        g_cur_dir_name[p_head_n],
        (strlen(g_cur_dir_name[p_head_n]) + strlen(p_dir) + 1) * sizeof(char));

    strcat(g_cur_dir_name[p_head_n], p_dir);
  }

  g_dir_count[p_head_n] =
      SafeScandir(g_cur_dir_name[p_head_n], &g_dirents[p_head_n], NULL,
                  TypeAlphaSort, __LINE__);
  // for (int i = 0; i < g_dir_count[p_head_n]; i++) {
  //   realpath(g_dirents[p_head_n][i]->d_name, g_dirents[p_head_n][i]->d_name);
  // }

  for (int i = 1; i < g_dir_count[p_head_n]; i++) {
    if (l_item_ino == g_dirents[p_head_n][i]->d_ino) {
      l_need_pos = i;
      l_need_name_search = 0;

      if (g_bef_fold_num[p_head_n] > 0) {

        free(g_bef_fold_name[p_head_n][g_bef_fold_num[p_head_n]]);

        if (g_bef_fold_num[p_head_n] == 1) {
          free(g_bef_fold_name[p_head_n]);
          g_bef_fold_name[p_head_n] = NULL;
        }

        g_bef_fold_num[p_head_n]--;
      }

      break;
    }
  }
  if (l_need_name_search == 1) {
    for (int i = 0; i < g_dir_count[p_head_n]; i++) {
      if (strcmp(g_bef_fold_name[p_head_n][g_bef_fold_num[p_head_n]],
                 g_dirents[p_head_n][i]->d_name) == 0) {

        if (g_bef_fold_num[p_head_n] > 0) {

          free(g_bef_fold_name[p_head_n][g_bef_fold_num[p_head_n]]);

          if (g_bef_fold_num[p_head_n] == 1) {
            free(g_bef_fold_name[p_head_n]);
            g_bef_fold_name[p_head_n] = NULL;
          }

          g_bef_fold_num[p_head_n]--;
        }

        l_need_pos = i;

        break;
      }
    }
  }

  ChDirList(p_head_n, g_dir_count[p_head_n], &g_dirents[p_head_n],
            g_cur_dir_name[p_head_n], l_need_pos);
  ChHeaderDirStr(p_head_n, g_cur_dir_name[p_head_n], 1);
}
