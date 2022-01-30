#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h> 

#define PRODUCERS 10
#define N 10

void* consumer(void *arg);
void* producer(void *id);

int write_pos, read_pos;
int buf[N];

//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t *empty;
sem_t *full;
sem_t *mutex;

int id[PRODUCERS];

void init() {
  //sem_init(&empty, 0, N);
  //sem_init(&full, 0, 0);

  sem_unlink("EMPTY");
  empty=sem_open("EMPTY",O_CREAT|O_EXCL,0700,N);
  sem_unlink("FULL");
  full=sem_open("FULL",O_CREAT|O_EXCL,0700,0);
  sem_unlink("MUTEX");
  mutex=sem_open("MUTEX",O_CREAT|O_EXCL,0700,1);
  write_pos = read_pos = 0;
}

int main(int argc, char *argv[]) {
  int i;
  init();
  pthread_t thr;
  pthread_create(&thr, NULL, consumer, NULL);

  for (i = 0; i < PRODUCERS; i++) {
    id[i] = i;
    pthread_create(&thr, NULL, producer, &id[i]);
  } 
  pthread_exit(NULL);
  return 0;
}

void* producer(void *id) {
  int my_id = *((int*) id);
  int i = my_id;

  while (1) {
    sem_wait(empty);
    sem_wait(mutex);
    //pthread_mutex_lock(&mutex);
    
    printf("[PRODUCER %3d] Writing %d\n", my_id, i);
    buf[write_pos] = i;
    write_pos = (write_pos+1) % N;

    sem_post(full);
    sem_post(mutex);
    //pthread_mutex_unlock(&mutex);
    ++i; 
  }
}

void* consumer(void *arg) {
  while (1) {
    sem_wait(full);
    sem_wait(mutex);
    //pthread_mutex_lock(&mutex);
    
    int e = buf[read_pos];
    read_pos = (read_pos+1) % N; 
    printf("[CONSUMER    ] Read %d\n", e);

    //pthread_mutex_unlock(&mutex);

    sem_post(empty); 
    sem_post(mutex);
  }
  return NULL;
}
