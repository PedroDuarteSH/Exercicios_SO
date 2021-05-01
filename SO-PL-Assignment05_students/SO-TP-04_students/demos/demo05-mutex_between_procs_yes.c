// Vasco Pereira 2017 (vasco@dei.uc.pt)

// Using mutexes between processes
// Mutex default attributes have to be changed and shared memory used

// Compile as: gcc -Wall -lpthread -D_REENTRANT mutex_between_procs_yes.c -o mbp_y

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <pthread.h>

#define N_PROCS 4

typedef struct {
	int var;
	pthread_mutex_t mutex;
	} rec_struct;

rec_struct* shared_rec;

int shmid, temp;
//pthread_mutex_t mutex;
pthread_mutexattr_t attrmutex;

void worker()
	{
	pthread_mutex_lock(&shared_rec->mutex);
	printf("Worker %d: started updating variable in shared memory\n",getpid());
	temp=(*shared_rec).var+1;
	sleep(rand()%3);
	(*shared_rec).var=temp;
	printf("Worker %d: finished updating - new value is %d\n",getpid(),(*shared_rec).var);
	pthread_mutex_unlock(&shared_rec->mutex);
	}

int main(int argc, char *argv[])
	{
	int i;


	if ((shmid = shmget(IPC_PRIVATE, sizeof(rec_struct), IPC_CREAT | 0766)) < 0) {
		perror("Error in shmget with IPC_CREAT\n");
		exit(1);
	}

	if ((shared_rec = (rec_struct *) shmat(shmid, NULL, 0)) == (rec_struct*)-1) {
		perror("Shmat error!");
		exit(1);
	}

	/* Initialize attribute to mutex. */
	pthread_mutexattr_init(&attrmutex);
	pthread_mutexattr_setpshared(&attrmutex, PTHREAD_PROCESS_SHARED);

	/* Initialize mutex. */
	pthread_mutex_init(&shared_rec->mutex, &attrmutex);


	shared_rec->var=1000; // initialize variable in shared memory

	for(i=0;i<N_PROCS;i++){
		if(fork()==0){
			printf("Worker %d created!\n",getpid());
			worker();
			printf("Worker %d leaving!\n",getpid());
			exit(0);
			}
		}
	for(i=0;i<N_PROCS;i++) wait(NULL); // parent waits for all children

	// print final result
	printf("Final result= %d\n",shared_rec->var);

	// remove resources
	pthread_mutex_destroy(&shared_rec->mutex);
	pthread_mutexattr_destroy(&attrmutex);
	shmdt(shared_rec);
	shmctl(shmid,IPC_RMID,NULL);

	exit(0);
}
