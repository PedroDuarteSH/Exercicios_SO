#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX 60		// game width
#define SIZE 10 	// road width
#define LINES 40 	// max game lines

int * shared_pos;
int shmid;
int direction = 1;
int left = 0;
void road();
void signals(int signum);
void ignore_signals(int signum);

int main() {
        int pid;
        struct sigaction act;

        // Create shared memory for car position
        if((shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0777)) == -1)
                printf("Failed to get shared memory");
        /* TO COMPLETE - Create shared memory to save car position (an integer is enough) */
        if((shared_pos = (int *) shmat(shmid, NULL, 0)) == -1)
                printf("Failed to attach shared memory ");
	// initial car position
        *shared_pos = SIZE/2;

        // Create child to handle road
        pid = fork();
        if(pid == 0) {
                road();
                exit(0);
        }

        //*** Father ***//
        // Handle signals
        signal(SIGINT, signals); //Ctrl C
        signal(SIGTSTP, signals); //Ctrl V
        /* TO COMPLETE - handle SIGINT and SIGTSTP to control the car - only one function ("signals") will handle both signals */
        // Wait for child
        
        /* TO COMPLETE - Wait for child - if a signal is received while waiting, the wait must continue */
        wait(NULL);
        printf("Child finished!\n");

        // Remove shared memory
        shmdt(shared_pos);
        shmctl(shmid,IPC_RMID,NULL);
}

void road() {
        struct sigaction act;

	// Ignore SIGINT and SIGTSTP
	// Configuring sigaction
        signal(SIGINT,SIG_IGN);
        signal(SIGTSTP,SIG_IGN);
        /* TO COMPLETE - Ignore SIGINT and SIGTSTP by using sigaction */

        for(int i = 0; i < LINES; i++) {
                if((*shared_pos) > left && (*shared_pos) < left+SIZE-1) {
			printf("%*s>%*sV%*s<\n", left, "", (*shared_pos)-left-1,"",SIZE+left-2-(*shared_pos),"");
			}
		else {
                        printf("%*cCRASH! GAME OVER!!!\n",left, ' ');
                        exit(0);
			}

                if(direction == 1) {
                        left++;
                } else {
                        left--;
                }

                if(left+SIZE >= MAX) {
                        direction = 0;
                } else if(left <= 0) {
                        direction = 1;
                } else {
                        // Make random direction change
                        if(rand()%10 == 0) {
                                if(direction == 0)
                                        direction = 1;
                                else
                                        direction = 0;
                        }

                }
                usleep(500*1000);
        }
	printf("END OF ROAD! WELL DONE!!!\n");
}

void signals(int signum) {

        /* TO COMPLETE - if SIGINT then increase the car position, if SIGTSTP decrease car position */
        if(signum == SIGINT) (*shared_pos)++;

        else (*shared_pos)--;


        printf("\n");
}
