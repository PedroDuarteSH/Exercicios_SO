#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int t_ini,num;
void wait_enter();
/* TO COMPLETE: Create the function that handles CTRL-Z */
void ctrlChandler(int signum){
	int time_elapsed = time(NULL) - t_ini;
	if(time_elapsed == num)printf("\nRecieved - Elapsed - %d - You got it right!\n", time_elapsed);
	else if(time_elapsed > num)printf("\nRecieved - Elapsed - %d - You were to slow!\n", time_elapsed);
	else printf("\nRecieved - Elapsed - %d - You were to fast!\n", time_elapsed);
	wait_enter();
}

/* function end */

/* TO COMPLETE: Create the function that handles CTRL-C */
void ctrlZhandler(int signum){
	printf("\nGoing out\n");
	fflush(stdout);
	exit(0);
}
/* function end */

int main(){
  	/* TO COMPLETE: Redirect signals */
	signal(SIGINT, ctrlChandler);
	signal(SIGTSTP, ctrlZhandler);

	/* End redirect */

  	// Do some work!
  	srand(time(NULL));
  	/* TO COMPLETE: Ignore the SIGINT (CTRL-C) */
	signal(SIGINT, SIG_IGN);
  	/* End ignore */
 	wait_enter();
  	while (1) {
		num=rand()%8+2;
		printf("\n\nPress CTRL-C in %d seconds!\nCountdown starting in 2 seconds... Get Ready!!\n",num);
		sleep(2);
		/* TO COMPLETE: Redirect the SIGINT again */
		signal(SIGINT, ctrlChandler);
		/* End redirect */

		printf("Start counting the seconds!!\n");
		t_ini=time(NULL);
  		pause();
	}
  return 0;
}

void wait_enter(){
	printf("\n- Press enter to continue -\n");
	while(getc(stdin) != 10);
}