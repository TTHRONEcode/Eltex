#include <pthread.h>
#include <stdio.h>

#define N 5

void *thread_calc(void *args) {
  int *i = (int *)args;
  printf("Thread num: %d\n", *i);

  return NULL;
}

int main() {
  int i, i1;
  int *thread_ext_status;
  int loc_i[N];

  pthread_t threads[N];

  for (i = 0; i < N; i++) {
    loc_i[i] = i;
    pthread_create(&threads[i], NULL, thread_calc, (void *)&loc_i[i]);
  }

  for (i1 = 0; i1 < N; i1++) {
    pthread_join(threads[i1], (void **)&thread_ext_status);
  }

  return 0;
}