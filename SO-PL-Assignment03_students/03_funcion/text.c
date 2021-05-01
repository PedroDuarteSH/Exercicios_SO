#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(void) {
  pid_t pid;
  int process = 4;
  int i = 0;
  for(i =0; i < process; i++){
  	pid = fork();
  	if(pid != 0){
  		if(i!= 0) printf("%d I am %d and my father\n", i, getpid());
  		else printf("%d i am %d, the original\n",i, getpid());
      wait(NULL);
  		exit(0);
  	}
  }
  printf("%d I am %d\n", i, getpid());
  return 0;
}
