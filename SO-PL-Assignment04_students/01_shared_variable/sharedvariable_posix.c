//compile with: gcc -Wall -pthread sharedvariable_posix.c -o svar
//using POSIX named semaphores

#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h> // include POSIX semaphores
#include <stdio.h>
/*insert here the other libraries needed*/
#include <stdlib.h>
#include <fcntl.h>

int * shared_var;
int shmid;
sem_t *mutex;

void worker()
	{
	usleep(1000000 + rand()%11*100000); //uses microseconds sleep and waits 1 to 2 seconds in periods of 0.1 secs
	sem_wait(mutex);
	(*shared_var)++;
	sem_post(mutex);
	}

int main(int argc, char *argv[])
	{
	int i, n_procs;
	if(argc!=2) {
		printf("Wrong number of parameters\n");
		exit(0);
	}

	n_procs=atoi(argv[1]);

	// Create shared memory
	if((shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0766)) < 0){
		printf("Error\n" );
		exit(0);
	}

	// Attach shared memory
	if((shared_var = (int *) shmat(shmid, NULL, 0)) == (int *)-1){
		printf("Error\n" );
		exit(0);
	}
	printf("%d\n", *shared_var);

	// Create semaphore
	sem_unlink("MUTEX");
	mutex=sem_open("MUTEX", O_CREAT|O_EXCL,0700,1);

	// initialize variable in shared memory
	/*insert code here*/
	*shared_var = 1000;
	// Create worker processes

	for(i = 0; i < n_procs; i++){
		if(fork() == 0){
			printf("worker %d created\n", getpid());
			worker();
			printf("worker %d leaving\n", getpid());
			exit(0);
		}
	}




	// Wait for all worker processes
	for(i = 0; i < n_procs; i++)wait(NULL);

	// print final result
	printf("%d\n", *shared_var);

	// remove resources (shared memory and semaphore)
	sem_close(mutex);
	sem_unlink("MUTEX");
	shmdt(shared_var);
	shmctl(shmid, IPC_RMID, NULL);

	exit(0);
}
