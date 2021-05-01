//compile with: gcc -Wall stockmarket.c semlib.c -o stock
//using SysV semaphores
//Note: you can change all the operations of semaphores to make it work with POSIX semaphores

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semlib.h"
#include <pthread.h>

#define NUM_WRITERS 5		// number of writers
#define NUM_READERS 5		// number of readers
#define STOCKLIST_SIZE 10  // stock list slots
#define MUTEX 0
#define STOP_WRITERS 1

typedef struct{
	int readers;
	int slots[STOCKLIST_SIZE];
} mem_structure;

int semid, shmid;
pthread_t my_thread[NUM_READERS + NUM_WRITERS];
int id[NUM_READERS + NUM_WRITERS];
mem_structure *stocklist;
pthread_mutex_t pthread_next_request = PTHREAD_MUTEX_INITIALIZER;

void cleanup(int signo) // clean up resources by pressing Ctrl-C
{
	int i = 0;
	while (i < (NUM_READERS + NUM_WRITERS))
		pthread_exit(NULL);

	if (semid >= 0)	// remove semaphores
		sem_close(semid);
	if (shmid >= 0) // remove shared memory
		shmctl(shmid, IPC_RMID, NULL);

	printf("Closing...\n");
	exit(0);
}

int get_stock_value()
{
	return 1 + (int) (100.0 * rand() / (RAND_MAX + 1.0));
}

int get_stock()
{
	return (int) (rand() % STOCKLIST_SIZE);
}

void write_stock(int n_writer, mem_structure *stocklist)
{
	int stock=get_stock();
	int stock_value=get_stock_value();
	stocklist->slots[stock] = stock_value;
	fprintf(stderr, "Stock %d updated by BROKER %d to %d\n", stock, n_writer, stock_value);
}

void *writer_code(void *arg)
{
	srand(getpid());
	int n_writer =*((int *)arg);
	while (1)
	{
		pthread_mutex_lock(&pthread_next_request);
		++(stocklist->readers);
		if((stocklist->readers) == 1){
			pthread_mutex_unlock(&pthread_next_request);
			pthread_exit(NULL);
		}
		write_stock(n_writer, stocklist);
		pthread_mutex_unlock(&pthread_next_request);

		sleep(1 + (int) (10.0 * rand() / (RAND_MAX + 1.0)));
	}

pthread_exit(NULL);
}

void read_stock(int pos, int n_reader, mem_structure *stocklist)
{
		fprintf(stderr, "Stock %d read by client %d = %d\n", pos, n_reader, stocklist->slots[pos]);
}

void *reader_code(void *arg)
{
	int n_reader =*((int *)arg);
	while (1)
	{
		pthread_mutex_lock(&pthread_next_request);
		++(stocklist->readers);
		if((stocklist->readers) == 1){
			pthread_mutex_unlock(&pthread_next_request);
			pthread_exit(NULL);
		}
		read_stock(get_stock(),n_reader, stocklist);
		pthread_mutex_unlock(&pthread_next_request);
		//sleep(1 + (int) (3.0 * rand() / (RAND_MAX + 1.0)));
		sleep(1);
	}
	pthread_exit(NULL);
}

void monitor() // main process monitors the reception of Ctrl-C
	{
	struct sigaction act;
	act.sa_handler = cleanup;
	act.sa_flags = 0;
	if ((sigemptyset(&act.sa_mask) == -1) ||
		(sigaction(SIGINT, &act, NULL) == -1))
		perror("Failed to set SIGINT to handle Ctrl-C");
	while(1){
		sleep(5);
		printf("Still working...\n");
		}
	exit(0);
	}


int main()
{

	//Create shared memory
	shmid = shmget(IPC_PRIVATE, sizeof(mem_structure), IPC_CREAT|0700);
	stocklist = (mem_structure*) shmat(shmid, NULL, 0);

	//Create semaphores
	stocklist->readers = 0;

	for (int i = 0; i < NUM_READERS; i++){
			id[i] = i+1;
		  pthread_create(&my_thread[i], NULL, reader_code, &id[i]);
	}

	for (int i = NUM_READERS; i < NUM_WRITERS+NUM_READERS; i++){
		id[i] = i+1;
		pthread_create(&my_thread[i], NULL, writer_code, &i);
	}


	for (int i = 0; i < NUM_READERS + NUM_WRITERS; i++){
		pthread_join(my_thread[i], NULL);
		printf("Joined\n");
	}
	monitor();
	exit(0);
}
