#include <dirent.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./9_2_graphic.h"

char name[80000] = {0}; // TODO заменить это нечто на malloc
struct dirent **dp;
int count;

static int TypeAlphaSort(const struct dirent **e_1, const struct dirent **e_2) {
  int c = 0;

  if ((*e_1)->d_type == DT_DIR) {
    if ((*e_2)->d_type == DT_DIR)
      c = strcoll((*e_1)->d_name, (*e_2)->d_name);
    else
      c = -1;

  } else {
    if ((*e_2)->d_type != DT_DIR)
      c = strcoll((*e_1)->d_name, (*e_2)->d_name);
    else
      c = 1;
  }

  return c;
}

void AAA() {}

static void FindAbsDir() { // TODO сделать обработку ВСЕХ ошибок
  ino_t this_dir_inode[20] = {0};
  char loc_name[10000] = {0};
  int inode_num = -1;

  strcat(loc_name, ".");
  count = scandir(loc_name, &dp, NULL, TypeAlphaSort);

  do {
    inode_num++;
    this_dir_inode[inode_num] = dp[0]->d_ino;

    strcat(loc_name, "/..");

    count = scandir(loc_name, &dp, NULL, TypeAlphaSort);

    for (int i = 1; i < count; i++) {
      if (this_dir_inode[inode_num] == dp[i]->d_ino) {

        // char_num += strlen(dp[i]->d_name);
        // strncpy(name + strlen(dp[i]->d_name) + 1, name, char_num);

        // name[strlen(dp[i]->d_name)] = '/';
        // // strncpy(name + strlen(dp[i]->d_name), "/", 2);

        // strncpy(name, dp[i]->d_name, strlen(dp[i]->d_name));

        break;
      }
    }

  } while (this_dir_inode[inode_num] != dp[0]->d_ino);
  name[0] = '/';
  for (int i = inode_num - 1; i >= 0; i--) {
    count = scandir(name, &dp, NULL, TypeAlphaSort);
    for (int j = 0; j < count; j++) {

      if (this_dir_inode[i] == dp[j]->d_ino) {
        strcat(name, dp[j]->d_name);
        strcat(name, "/");
        break;
      }
    }
  }

  // strcat(name, loc_name);

  // strcat(name, ".");

  // ./..
  // ./
  // ./9_2
}

void PrintDir() {
  FindAbsDir();

  ChList(0, count, dp, name);

  // free(dp); // TODO
}
