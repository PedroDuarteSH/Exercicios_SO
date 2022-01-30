/************************************************************************************
* FILE: count.c
* Compile as : gcc count_solution.c -lpthread -D_REENTRANT -Wall -o count
* DESCRIPTION:
*   NUM_WORKER_THREADS threads increase a counter a random number of times between 2 and 5
*   1 monitor thread watches the end of each worker thread
*   When all threads finish, the monitor thread prints the counter and leaves
*************************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_WORKER_THREADS  5

int count = 0;
int exited_threads=0;

/* initialize count_mutex and count_threshold_cv condition variable */
pthread_cond_t count_threshold_cv  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

/*insert code here - mutex and condition variable*/

void *increment_count(void *t) 
{
  long my_id = (long)t;
  int loops,i;

  loops=rand()%4+2; // increments a number between 2 and 5 times
  printf("Thread %ld started incrementing counter - will do it %d times\n", my_id,loops);

  /*insert code here - increment counter and wait 1 sec between each change*/
  for (int i = 0; i < loops; i++)
  {
    pthread_mutex_lock(&count_mutex);
    count++;
    pthread_mutex_unlock(&count_mutex);
  }
  
  /*insert code here - notify monitor thread before leaving*/

  printf("Thread %ld leaving!\n", my_id);
  pthread_exit(NULL);
}

void *watch_threads(void *t) 
{
  long my_id = (long)t;

  printf("Thread %ld started watching threads\n", my_id);

  /*insert code here - when all threads leave print the counter*/

  pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
  int i;
  long id[NUM_WORKER_THREADS+1];
  pthread_t threads[NUM_WORKER_THREADS+1];

  srand(time(NULL));

  /*insert code here - create threads*/
  phtread_create(threads[0], NULL, watch_threads, 1);
  for (int i = 1; i < NUM_WORKER_THREADS+1; i++)
  {
    id[i] = i+1;
    phtread_create(threads[i], NULL, increment_count, &id[i]);
  }

  /*insert code here - wait for all threads to complete*/
  for (int i = 0; i < NUM_WORKER_THREADS+1; i++)
  {
    pthread_join(threads[i], NULL);
  }
  
  /*insert code here - clean up and exit*/

}

