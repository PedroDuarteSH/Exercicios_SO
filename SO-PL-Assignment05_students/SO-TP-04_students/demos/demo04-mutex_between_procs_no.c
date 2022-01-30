// Vasco Pereira 2017 (vasco@dei.uc.pt)

// Mutexes created using static initialization cannot be used between processes

// Compile as: gcc -Wall -lpthread -D_REENTRANT mutex_between_procs_no.c -o mbp_n

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

int * shared_var;
int shmid, temp;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

void worker()
	{
	pthread_mutex_lock(&mutex);
	printf("Worker %d: started updating variable in shared memory\n",getpid());
	temp=*shared_var+1;
	sleep(rand()%3);
	*shared_var=temp;
	printf("Worker %d: finished updating - new value is %d\n",getpid(),*shared_var);
	pthread_mutex_unlock(&mutex);
	}

int main(int argc, char *argv[])
	{
	int i;


	if ((shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0766)) < 0) {
		perror("Error in shmget with IPC_CREAT\n");
		exit(1);
	}

	if ((shared_var = (int *) shmat(shmid, NULL, 0)) == (int*)-1) {
		perror("Shmat error!");
		exit(1);
	}


	*shared_var=1000; // initialize variable in shared memory

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
	printf("Final result= %d\n",*shared_var);

	// remove resources
	shmdt(shared_var);
	shmctl(shmid,IPC_RMID,NULL);	

	exit(0);
}

