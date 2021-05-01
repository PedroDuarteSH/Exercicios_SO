// Producer/Consumer using SysV semaphores - using the semlib library
// Buffer with N slots
// 1 producer and 1 consumer
// Compile as: gcc -Wall sem_test-sysv.c semlib.c -o sem_test-sysv

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semlib.h"

#define EMPTY 0
#define FULL 1
#define MUTEX 2
#define N 5     // N is the number of slots
#define TOTAL_VALUES 10
typedef struct {
	int buf[N];
	int write_pos, read_pos;
} mem_struct;

int shmid, sem;
int *write_pos, *read_pos, *buf;
mem_struct* mem;

void init() {
  shmid = shmget(IPC_PRIVATE, sizeof(mem_struct), IPC_CREAT|0700);
  if (shmid < 1) exit(0);
  mem = (mem_struct*) shmat(shmid, NULL, 0);
  if (mem < (mem_struct*) 1) exit(0);

  sem = sem_get(3, 0);
  sem_setvalue(sem, EMPTY, N);
  sem_setvalue(sem, FULL,  0);
  sem_setvalue(sem, MUTEX, 1);

  mem->write_pos = mem->read_pos = 0; //does not need to be in shm as each process only works with one variable, not changing the other (is necessary if there are more than 1 producer or 1 consumer)
  write_pos = &mem->write_pos;
  read_pos = &mem->read_pos;
  buf = (int*)&mem->buf;
}

void terminate() {
  sem_close(sem);
  shmctl(shmid, IPC_RMID, NULL);
}

void put_element(int e) {
  sem_wait(sem, EMPTY);
  sem_wait(sem, MUTEX);

  mem->buf[*write_pos] = e;
  *write_pos = (*write_pos+1) % N;

  sem_post(sem, MUTEX);
  sem_post(sem, FULL);
}

int get_element() {
  sem_wait(sem, FULL);
  sem_wait(sem, MUTEX);

  int e = mem->buf[*read_pos];
  *read_pos = (*read_pos+1) % N;

  sem_post(sem, MUTEX);
  sem_post(sem, EMPTY);

  return e;
}

void producer() {
  int i;
  for (i=TOTAL_VALUES; i>0; i--) {
    printf("[PRODUCER] Writing %d\n", i);
    put_element(i);
  }
}

void consumer() {
  int i;
  for (i=0; i<TOTAL_VALUES; i++) {
    int e = get_element();
    printf("[CONSUMER] Retrieved %d\n", e);
    sleep(1);
  }
  terminate();
}

int main(int argc, char* argv[]) {
  init();

  if (fork() == 0) {
    producer();
    exit(0);
  }
  else {
    consumer();
    exit(0);
  }
}
