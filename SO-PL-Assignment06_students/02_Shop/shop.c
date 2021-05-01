/******************************************************************************
* FILE: shop.c
* DESCRIPTION:
*   A shop has several moneyiers and one foreman, each represented by a thread. 
*   The threads are created by the main thread.
*   - Some threads represent the shop employees that sell items for 10 eur
*     and put the money in a common safe.
*   - One thread represents the foreman that watches the value of the money. When it 
*     reaches a predefined limit (CASH_LIMIT), he removes an amount of CASH_LIMIT
*     money from the safe, for security reasons.
*   - The program ends after NSALES
******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS  8	// Total number of threads (>1)
#define NSALES 49	// Max number of sales made by all employees
#define CASH_LIMIT 50	// Limit of the 'count' variable before it is reset 

int   sales = 0;
int		money = 0;
int   safe = 0;
/* initialize money_mutex and money_amount_cv condition variable */
pthread_cond_t money_amount_cv  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t money_mutex = PTHREAD_MUTEX_INITIALIZER;
// ...... insert code here .......

void *employee(void *t)
{
  int my_id = *((int *)t);
  
  printf("Starting employee(): thread %d\n", my_id);

  while (1) {
    pthread_mutex_lock(&money_mutex);
    if(sales < NSALES){
      money += 10; //sale
      sales++;
      pthread_cond_signal(&money_amount_cv);
      printf("SELL %d: Employee %d selled a new product\n", sales, my_id);
      
    }
      
	/* If sales < NSALES make a sell and put money in the safe
	   else exits thread */
    else{
      pthread_mutex_unlock(&money_mutex);
      pthread_exit(NULL);
    }
    pthread_mutex_unlock(&money_mutex);
    sleep(1);
	// ...... insert code here .......
	}

  pthread_exit(NULL);
}

void *foreman(void *t) 
{
  int my_id = *((int *)t);

  printf("Starting foreman(): thread %d\n", my_id);

  while (1) {
    /* wait if money<CASH_LIMIT and sales<NSALES
	   when sales >= NSALES exit thread */
    pthread_mutex_lock(&money_mutex);
    while(money < CASH_LIMIT && sales < NSALES){
      pthread_cond_wait(&money_amount_cv, &money_mutex);
    }
    if(money >= CASH_LIMIT){
      money -= CASH_LIMIT;
      printf("Foremad removed money:\nAmount: %d\n", money);
    }
    if(sales >= NSALES){
      pthread_mutex_unlock(&money_mutex);
      break;
    }
    
    pthread_mutex_unlock(&money_mutex);
	// ...... insert code here .......

	}
 
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  int i = 0; 
  int id[NUM_THREADS];
  pthread_t threads[NUM_THREADS];

  /* Create threads */
  id[i] = 1;
  pthread_create(&threads[i], NULL, foreman, &id[i]);
  for (i = 1; i < NUM_THREADS; i++)
  {
    id[i] = i+1;
    pthread_create(&threads[i], NULL, employee, &id[i]);
  }
  
  // ...... insert code here .......
  for (i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(threads[i], NULL);
  }
  
  /* Wait for all threads to complete */

  // ...... insert code here .......

  printf ("Main(): Waited and joined with %d threads. Final money in safe = %d. Done!\n", 
          NUM_THREADS, money);

  /* Clean up and exit */
  pthread_cond_destroy(&money_amount_cv);
  pthread_mutex_destroy(&money_mutex);
  exit(0);
  // ...... insert code here .......

}
