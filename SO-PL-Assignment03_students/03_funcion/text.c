#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
  pid_t pid;
  int process = 5;
  int i = 0;
  for(i =0; i < process-1; i++){
  	pid = fork();
  	if(pid != 0){
      wait(NULL); //Esperamos que o proximo processo gerado execute
  		if(i!= 0) printf("I am %d and my father is %d\n", getpid(), getppid());
  		else printf("I am %d, the original\n", getpid());
  		exit(0);
  	}
  }
  printf("I am %d and my father is %d\n", getpid(), getppid());
  return 0;
}
