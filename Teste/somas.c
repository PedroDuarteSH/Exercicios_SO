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
#include <pthread.h>

#define PIPE_NAME1 "PIPE1" 
#define PIPE_NAME2 "PIPE2" 

// variáveis globais 
int fd_named_pipe[2]; 
int soma_total=0;
int leituras_real = 0;
pthread_t threads[2];

int term = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void sig(int signum){
    if ((fd_named_pipe[0] = open(PIPE_NAME1, O_WRONLY)) < 0){
        perror("Cannot open pipe for writing: ");
        exit(0);
    }
    const int num = -1;
    write(fd_named_pipe[0], &num, sizeof(int));
}


void *estatisticas(){
    while(1){
        pthread_mutex_lock(&mutex);
        printf("Numero de leituras: %d\nSoma Total: %d\n", leituras_real, soma_total);
        if(term == 1){
            pthread_exit(NULL);
            pthread_mutex_unlock(&mutex);
        }
        pthread_mutex_unlock(&mutex);
        sleep(5);
    }

}

void *le_dados(){
    fd_set read_set;
    int r;
    while(1){
        if ((fd_named_pipe[0] = open(PIPE_NAME1, O_RDONLY | O_NONBLOCK)) < 0){
            perror("Cannot open pipe for reading: ");
            exit(0);
        }
        if ((fd_named_pipe[1] = open(PIPE_NAME2, O_RDONLY | O_NONBLOCK)) < 0){
            perror("Cannot open pipe for reading: ");
            exit(0);
        }
        FD_ZERO(&read_set);
        FD_SET(fd_named_pipe[0], &read_set);
        FD_SET(fd_named_pipe[1], &read_set);
        if(select(fd_named_pipe[1]+1, &read_set, NULL, NULL, NULL) > 0){
            if(FD_ISSET(fd_named_pipe[0], &read_set)){
                read(fd_named_pipe[0], &r, sizeof(int));
                if(r == -1){
                    term++;
                    pthread_exit(NULL);
                }
                pthread_mutex_lock(&mutex);
                soma_total += r;
                leituras_real++;
                pthread_mutex_unlock(&mutex);
            }
            if(FD_ISSET(fd_named_pipe[1], &read_set)){
                read(fd_named_pipe[1], &r, sizeof(int));
                if(r == -1){
                    pthread_mutex_lock(&mutex);
                    term++;
                    pthread_mutex_unlock(&mutex);
                    pthread_exit(NULL);
                }
                pthread_mutex_lock(&mutex);
                soma_total+= r;
                leituras_real++;
                pthread_mutex_unlock(&mutex);
            }
        }
    }
}

int main(){
    signal(SIGINT, sig);
    //Criar os named pipes
    if((mkfifo(PIPE_NAME1, O_CREAT|O_EXCL|0600) < 0) && (errno != EEXIST)){
        perror("Cannot create named pipe: ");
        exit(0);
    }

    if((mkfifo(PIPE_NAME2, O_CREAT|O_EXCL|0600) < 0) && (errno != EEXIST)){
        perror("Cannot create named pipe: ");
        exit(0);
    }

    //Criar threads
    pthread_create(&threads[0], NULL, le_dados, NULL);
    pthread_create(&threads[1], NULL, estatisticas, NULL);

    //Esperar finalização threads
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    pthread_mutex_destroy(&mutex);
    close(fd_named_pipe[0]);
    close(fd_named_pipe[1]);
    unlink(PIPE_NAME1);
    unlink(PIPE_NAME2);
    exit(0);
}
    //Abrir os names pipes

