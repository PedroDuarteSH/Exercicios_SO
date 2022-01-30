#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)



int width;
int fddst; //Destination file descriptor
char *dst; //Destination mmap
int header_len;
int size;
int lines = 0;
sem_t l;

int **image;

void writeblack(){
    int pos = header_len;
    int temp_lines = lines;
    int square_num = 0;
    while(pos < size){      
        square_num++;
        printf("Black number %d from pos %d to %d\n", square_num, (pos-header_len)/2, (pos-header_len)/2+2);
        sem_wait(&l);
        if(temp_lines != lines){
            temp_lines = lines;
            pos++;
        }
        sem_post(&l);
        sem_wait(&l);
        if((lines+1) % (width/3) == 0 && lines != 0)pos+=width/3 * 2;
        sem_post(&l);
        for (int i = 0; i < width/3; i++){
            dst[pos++] = '1';
            if((pos-header_len+1) % (2*width) == 0){
                sem_wait(&l);
                dst[pos++] = '\n';
                temp_lines++;
                lines++;
                sem_post(&l);
            }
            else dst[pos++] = ' ';
        }
        if(square_num % 2 == 0);
        else
            pos+=width/3 * 2;
        //if((lines+1) % width/3 == 0)pos+=width/3 * 2;
    }
    exit(0);
}
void writewhite(){
    int pos = header_len+ width/3 * 2;
    int temp_lines = lines;
    int square_num = 0;
    while(pos < size){
        square_num++;
        printf("White number %d from pos %d to %d\n", square_num, (pos-header_len)/2, (pos-header_len)/2+2);
        sem_wait(&l);
        if(temp_lines != lines){
            temp_lines = lines;
            pos++;
        }
        sem_post(&l);    
        sem_wait(&l);
        if((lines+1) % (width/3) == 0 && lines != 0)pos+=width/3 * 2;
        sem_post(&l); 
        for (int i = 0; i < width/3; i++){
 
            dst[pos++] = '0';
            if((pos-header_len+1) % (2*width) == 0){
                dst[pos++] = '\n';
                temp_lines++;
                sem_wait(&l);
                lines++;
                sem_post(&l);
            }
            else dst[pos++] = ' ';
        }
        if(square_num % 2 == 0);
        else
            pos+=width/3 * 2;
        //if((lines+1) % width/3 == 0)pos+= width/3 * 2;
        
    }
    exit(0);
}

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("usage: ./prog <dst file name> <width=height> \n");
        exit(1);
    }
    sem_init(&l, 1, 1);

    width = atoi(argv[2]);
    header_len = 3 + 2 * strlen(argv[2]) + 2 + 2;
    size = (2*width)*width + header_len + width*2-1;
    char *out = malloc(sizeof(char) * strlen(argv[1]) + 5);
    strcpy(out, argv[1]);
    //Open/Create destination file
    if((fddst = open (strcat(out, ".ppm"), O_RDWR | O_CREAT | O_TRUNC,FILE_MODE)) < 0){
      fprintf(stderr,"dst open error: %s\n",strerror(errno));
      exit(1);
    }

    lseek(fddst, size - 1, SEEK_SET);
    // put something there
    write(fddst, "" , 1);

    //Map file to dst array;
    if ((dst = mmap(0, size, PROT_READ | PROT_WRITE,MAP_SHARED,fddst, 0)) == (caddr_t) -1){
		fprintf(stderr,"mmap error for output\n");
		exit(1);
	}



    //write header (name and height and withd)
    strcat(dst, "P6\n");
    strcat(dst, argv[2]);
    strcat(dst, " ");
    strcat(dst, argv[2]);
    strcat(dst, "\n1\n");


    if(fork() == 0)writeblack();
    if(fork() == 0)writewhite();


    wait(NULL);
    wait(NULL);

    munmap(dst,size);
    exit(0);
}