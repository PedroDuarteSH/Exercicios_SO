#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#define CASHIERS_NUM 3

typedef struct {
	long mtype;
	int customer_id;
	int num_products;
} customer_msg;

int mq_id;
pid_t proc_ori;

void cleanup(int signum) {
	printf("\n[%d] Cleanup and exit!\n", getpid());

	/* TO COMPLETE: Cleanup - Remove MSQ */
  	if(getpid() == proc_ori)
	  msgctl(mq_id, IPC_RMID, NULL);

	// End process
	exit(0);
}

// Generate customer requests
void generateCustomers() {
	printf("[%d] Store is now open!\n", getpid());

	srand(time(NULL));
	int customer_id = 0;
	customer_msg customer;
	while (1) {
		
		long mtype = rand()%CASHIERS_NUM+1; // Select between available cashiers (mtype>0)
		int num_products = rand()%3+2; // Products between 2 and 5

		/* TO COMPLETE: Create customer */
		customer.mtype = mtype;
		customer.num_products = num_products;
		customer.customer_id = customer_id;
		/* TO COMPLETE: Print customer details */
		printf("[%d] Customer %d arrived and selected cashier %ld (%d products).\n",getpid(),customer_id, mtype, num_products);
		/* TO COMPLETE: Add customer to Message Queue */
		msgsnd(mq_id, &customer, sizeof(customer_msg)-sizeof(long), 0);
		// Time between customers arrival
		sleep(rand()%5+1);

		//increment customer number
		customer_id++;
	}
}

// Cachiers process
void cashier(int i) {
	printf("[%d] Cashier %d open!\n", getpid(), i);

	customer_msg customer;

	while (1) {
		// Process next customer in cachier queue
		printf("[%d] Cashier %d waiting for next customer.\n", getpid(), i);

		/* TO COMPLETE: Get next customer from message queue */
		msgrcv(mq_id,&customer,sizeof(customer_msg) - sizeof(long),i,0);
		/* TO COMPLETE: Print customer details */
 		printf("[%d] > Cashier %d handling customer %d (%d products)!\n", getpid(), i, customer.customer_id, customer.num_products);
		/* TO COMPLETE: Process customer products */
		sleep(customer.num_products);

	}
	exit(0);
}

int main(int argc, char* argv[]) {


	proc_ori=getpid(); // save the original proc PID so that only one destroys de MSQ

	/* TO COMPLETE: Handle ^C */
	signal(SIGINT, cleanup);
	/* TO COMPLETE: Create Message Queue */
	mq_id = msgget(IPC_PRIVATE, IPC_CREAT|0777);
  	if(mq_id == -1){
   	 	printf("Error creating message queue");
    	exit(0);
    }
	/* TO COMPLETE: Create cashiers processes */
	for (int i = 0; i < CASHIERS_NUM; i++)
		if(fork() == 0)
			cashier(i+1);
	/* TO COMPLETE: Generate customer requests */
	generateCustomers();
}
