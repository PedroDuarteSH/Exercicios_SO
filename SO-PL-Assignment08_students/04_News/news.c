#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#define NUMBER_TOPICS 5
typedef struct{
    long topic;
    int crew;
}new;

char topics[NUMBER_TOPICS][100] = {"Economy", "Culture", "Sports", "Science", "Technology"};
int mqid;
pid_t proc_ori;

void cleanup(int signo){
    if(getpid() == proc_ori){
        printf("\n[%d] Cleanup and exit!\n", getpid());
        //Clear message queue
        msgctl(mqid, IPC_RMID, NULL);
    }else printf("->[%d] DEAD!\n", getpid());
	  

	exit(0);
}

void news_crew(int crew_number){
    srand(getpid());
    int topic;
    new gen_new;
    gen_new.crew = crew_number;

    while(1){
        //generate a topic
        topic = rand() % 5;

        gen_new.topic = topic;
        printf("[%d] News Crew %d find new of %s!\n", getpid(),crew_number, topics[topic]);
        //Send new through message queue
        msgsnd(mqid, &gen_new, sizeof(new)-sizeof(long), 0);

        sleep(rand() % 3 + 1);
    }
    exit(0);
}

void editors(int topic){
    printf("[%d] Editor from topic %s created!\n", getpid(), topics[topic]);
    new rec_new;
    while(1){
        printf("[%d] Editor of %s receiving news!\n", getpid(), topics[topic]);
        msgrcv(mqid,&rec_new,sizeof(new) - sizeof(long),topic,0);  
        if((rand() % 100 + 1) < 10)//publish the new
            printf("[%d] Editor of %s pubished new recieved new from %d team!\n", getpid(), topics[topic], rec_new.crew);
        else printf("[%d] Editor of %s refused publish new recieved new from %d team!\n", getpid(), topics[topic], rec_new.crew);
    
    }
    exit(0);
}


int main(int argc, char* argv[]){
    proc_ori = getpid();
    /* Handle ^C */
	signal(SIGINT, cleanup);
	/* Create Message Queue */
	if((mqid = msgget(IPC_PRIVATE, IPC_CREAT|0777)) == -1){
   	 	printf("Error creating message queue");
    	exit(0);
    }
    //Create Editors
    for (int i = 0; i < 5; i++){
        if(fork() == 0)editors(i);
    }
    for (int i = 1; i < 10; i++){
        if(fork() == 0)news_crew(i+1);
    }
    news_crew(10);

    cleanup(SIGINT);
}
