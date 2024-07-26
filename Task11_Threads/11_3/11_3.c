#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SHOPS_N 5
#define BUYERS_N 3

#define handle_error_en(en, msg)                                               \
  do {                                                                         \
    errno = en;                                                                \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_once_t once_loader = PTHREAD_ONCE_INIT;

/* 0 - no, 1 - yes */
int is_occuiped[SHOPS_N];
int shop_products[SHOPS_N];
int loader_power = 50000;

int s[BUYERS_N + 1];
pthread_t loader_thrd;

void *Loader(void *args) {
  int l_s;

  l_s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  if (l_s != 0)
    handle_error_en(l_s, "pthread_setcancelstate");

  while (1) {

    for (int i = 0; i < SHOPS_N; i++) {
      pthread_mutex_lock(&m1);
      if (is_occuiped[i] == 0) {
        is_occuiped[i] = 1;

        printf("\n"
               "[           Тар-машина            ]\n"
               "[ Разгрузил %4d в %1d магаз       ]\n"
               "[ Там было: %6d, стало: %6d ]\n"
               "[ Разгрузка закончена, иду спать  ]\n",
               loader_power, i, shop_products[i],
               shop_products[i] + loader_power);
        shop_products[i] += loader_power;

        is_occuiped[i] = 0;
        pthread_mutex_unlock(&m1);
        sleep(1);
      } else {
        pthread_mutex_unlock(&m1);
      }
    }
  }

  return NULL;
}

void InitLoader(void) {
  s[BUYERS_N] = pthread_create(&loader_thrd, NULL, Loader, NULL);
  if (s[BUYERS_N] != 0)
    handle_error_en(s[BUYERS_N], "loader pthread_create");
}

void *Buyers(void *args) {

  pthread_once(&once_loader, InitLoader);

  int thrd_n = *((int *)args);
  int needs = 100000 + (rand() % 10000 - 5000);

  while (needs > 0) {
    for (int i = 0; i < SHOPS_N; i++) {
      pthread_mutex_lock(&m1);
      if (is_occuiped[i] == 0) {
        is_occuiped[i] = 1;

        needs -= shop_products[i];

        printf("\n"
               "*Я %d поток-покупатель, моя потребность была: %d.\n"
               " Залетаю в магазин №%d, там было %d товаров\n"
               "_Теперь моя потребность %d, иду спать..."
               "\n",
               thrd_n, needs + shop_products[i], i, shop_products[i],
               needs >= 0 ? needs : 0);

        shop_products[i] = needs < 0 ? shop_products[i] - (needs) : 0;
        if (needs <= 0) {
          printf(" ...не иду - я ж уже закупился.\n"
                 "_Всем пока!)\n");
          pthread_mutex_unlock(&m1);
          return NULL;
        }
        pthread_mutex_unlock(&m1);
        sleep(2);

        is_occuiped[i] = 0;
      }
      pthread_mutex_unlock(&m1);
    }
  }

  return NULL;
}

int main() {
  int i, i1;

  int *thrd_ext_status;
  int loc_i[BUYERS_N];

  pthread_t buyers_thrd[BUYERS_N];

  srand(time(NULL));

  printf("\n* [ Операция: Контрольная закупка ] *\n");

  for (int i = 0; i < SHOPS_N; i++) {
    shop_products[i] = 10000 + (rand() % 2000 - 1000);
  }

  for (i = 0; i < BUYERS_N; i++) {
    loc_i[i] = i;
    s[i] = pthread_create(&buyers_thrd[i], NULL, Buyers, (void *)&loc_i[i]);
    if (s[i] != 0)
      handle_error_en(s[i], "pthread_create");
  }

  for (i1 = 0; i1 < BUYERS_N; i1++) {
    s[i1] = pthread_join(buyers_thrd[i1], (void **)&thrd_ext_status);
    if (s[i1] != 0)
      handle_error_en(s[i1], "loader pthread_join");
  }

  s[BUYERS_N] = pthread_cancel(loader_thrd);
  if (s[BUYERS_N] != 0)
    handle_error_en(s[BUYERS_N], "loader pthread_cancel");

  s[BUYERS_N] = pthread_join(loader_thrd, (void **)&thrd_ext_status);
  if (s[BUYERS_N] != 0)
    handle_error_en(s[BUYERS_N], "loader pthread_join");

  exit(EXIT_SUCCESS);
}