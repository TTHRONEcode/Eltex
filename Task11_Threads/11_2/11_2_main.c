#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define THREADS_N 4
#define N LONG_MAX / 1000000000

long a;
long temp[THREADS_N];
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;

void no_thread() {
  for (long i = 0; i < N; i++) {
    a++;
  }
}

void *thread_calc_mutex(void *args) {

  for (long i = 0; i < N / THREADS_N; i++) {
    pthread_mutex_lock(&m1);
    a++;
    pthread_mutex_unlock(&m1);
  }

  return NULL;
}

void *thread_calc_n_mutex(void *args) {
  int n = *((int *)args);
  long loc_temp = 0;

  for (long i = 0; i < N / THREADS_N; i++) {
    loc_temp++;
  }
  temp[n] = loc_temp;

  return NULL;
}

int main() {
  long i, i1;
  int *thread_ext_status;
  int choise;
  int personal_i[THREADS_N];

  pthread_t threads[THREADS_N];

  time_t start_time, end_time;
  double elapsed_time;

  printf("*Need to calc 'a' to %ld*\n"
         "*Enter calc type:\n"
         "*1. Single Thread\n"
         "*2. %d-Thread with mutex [ !FATAL TIME WASTE WARNING! ]\n"
         "*3. %d-Thread witout mutex\n"
         "*_. Exit\n"
         "* -> ",
         N, THREADS_N, THREADS_N);
  scanf("%2d", &choise);

  if (choise != 1) {

    for (i = 0; i < THREADS_N; i++) {
      personal_i[i] = i;
      switch (choise) {
      case 2:
        pthread_create(&threads[i], NULL, thread_calc_mutex, NULL);
        break;

      case 3:
        pthread_create(&threads[i], NULL, thread_calc_n_mutex, &personal_i[i]);
        break;

      default:
        printf("*Exiting...\n");
        exit(0);
        break;
      }
    }
  } else {
    start_time = time(0);
    no_thread();
  }
  if (choise != 1)
    start_time = time(0);

  for (i1 = 0; i1 < THREADS_N; i1++) {
    pthread_join(threads[i1], (void **)&thread_ext_status);
  }
  if (choise == 3) {
    for (i = 0; i < THREADS_N; i++) {
      a += temp[i];
    }
  }

  end_time = time(0);
  elapsed_time = difftime(end_time, start_time);

  printf("a == %ld\n", a);
  printf("Elapsed time: %f seconds\n", elapsed_time);

  return 0;
}