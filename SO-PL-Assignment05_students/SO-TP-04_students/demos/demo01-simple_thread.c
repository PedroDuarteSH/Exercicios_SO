#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 10


pthread_t my_thread[N];
int id[N];

void *worker(void* idp) {
  int my_id = *((int *)idp);

  printf("Hello, I'm thread %d\n", my_id);
  sleep(rand()%3);
  printf("Hello, I'm thread %d, going away!\n", my_id);

  pthread_exit(NULL);
  return NULL;
}

int main(void) {
  int i;
  // create N threads
  for (i = 0; i < N; i++) {
    id[i] = i;
    pthread_create(&my_thread[i], NULL, worker, &id[i]);
  }

  //waits for them to die
  for (i = 0; i < N; i++) {
    pthread_join(my_thread[i], NULL);
  }
  exit(0);
}

