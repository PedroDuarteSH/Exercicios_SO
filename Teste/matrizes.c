#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h> // include POSIX semaphores
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define TOTAL_MATRIZES 3

typedef struct data{
	int matriz[10][10];
	int n_matriz;
	int n_processados;
}mems;
//estrutura para a SM
mems *mems_ptr;
//ponteiro para a SM

sem_t *worker_sem;
//semaforo a utilizar
sem_t *gerador_sem;

//semaforo a utilizar

int randomNumber(){
	return rand() % 10;
}

void consumidor(int n){
	int i;
	int j;
	int contador;
	int matrizactual=0;
	while(1){
		sem_wait(worker_sem);
		if(matrizactual==mems_ptr->n_matriz){
			sem_post(worker_sem);
			if(matrizactual==TOTAL_MATRIZES)
				exit(0);
            
			sleep(1);
		}
		else{
            contador=0;
			for(i =0 ; i<10;i++){
				for(j=0;j<10;j++){
					if(mems_ptr->matriz[i][j]==n){
						contador++;
					}
				}
			}
			mems_ptr->n_processados++;
			matrizactual=mems_ptr->n_matriz;
			if(mems_ptr->n_processados==10){
				sem_post(gerador_sem);
			}
			printf("Eu conto %ds e contei %d\n", n,contador);
			sem_post(worker_sem);
		}
	}
}

void gerador(){
	//no exercicio não é IPC_PRIVATE, é 1234567
	int i;
	int j;
	int w;
	for(w=0;w<TOTAL_MATRIZES;w++){
		sem_wait(gerador_sem);
		printf("Vou gerar\n");
		mems_ptr->n_processados=0;
		mems_ptr->n_matriz=w+1;
		for(i=0;i<10;i++){
			for(j=0;j<10;j++){
				mems_ptr->matriz[i][j]=randomNumber();
			}
		}
		sem_post(worker_sem);
	}
	exit(0);
}

void init(){

	int shmid=shmget(IPC_PRIVATE, sizeof(mems),IPC_CREAT|0700); //obtem o identificador para SM com o tamanho de infor

	mems_ptr= (mems*) shmat(shmid, NULL, 0); //mapeia a SM

	worker_sem=sem_open("sem1",O_CREAT|O_EXCL,0700, 0); //cria um named semaphore com o nome "sem"
	gerador_sem=sem_open("sem2",O_CREAT|O_EXCL,0700, 1); //cria um named semaphore com o nome "sem"

	mems_ptr->n_processados=0;

	for(int w=0;w<11;w++){
		//cria n_vectores processos para trabalhar, que no fim de o fazer vão acabar
		if(fork()==0){
			if(w==0) gerador();
			else consumidor(w-1);

			exit(0);
		}
	}

	int i;
    for(i=0; i<11; i++)wait(NULL);

	sem_close(worker_sem); //fecha o semaforo
	sem_unlink("sem1"); //detach do semaforo
	sem_close(gerador_sem);	//fecha o semaforo
	sem_unlink("sem2"); //detach do semaforo
	shmdt(mems_ptr); //detach da SM
	shmctl(shmid, IPC_RMID, NULL); //eliminação da SM
}


int main(int argc, char ** argv){
	init();
	return 0;

}