// Compile as: gcc matrix.c -lpthread -D_REENTRANT -Wall -o matrix

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define M 3 //first matrix lines
#define K 2 //first matrix columns; second matrix lines
#define N 3 //second matrix columns

struct v {
  int row;
  int column;
};

typedef struct v coordenate;
void *worker(void* coord);
void *show_matrix();	// prints matrices on screen

pthread_t my_thread[(M*N)+1];
int A[M][K] = { {1,4}, {2,5}, {3,6} };
int B[K][N] = { {8,7,6}, {5,4,3} };
int C[M][N];

// creates mutex
pthread_mutex_t pthread_next_request = PTHREAD_MUTEX_INITIALIZER;



void print_matrix(){
// prints matrices on screen when having exclusive access
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      printf("%d ", C[i][j]);
    }
    printf("\n");
  }
}

int main(void) {

  // creates threads and sends each the coordinates of the element to calculate
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      coordenate *coord = malloc(sizeof(coordenate));
      coord->row = i;
      coord->column = j;

      pthread_create(&my_thread[i], NULL, worker, coord);
    }
  }
  //Create thread to print
  pthread_create(&my_thread[N * M], NULL, show_matrix, NULL);
  // waits for threads to finish
  for (int i = 0; i < N*M+1; i++) pthread_join(my_thread[i], NULL);
  // prints final results of the calculation and leaves
  printf("Finish Matrix\n");
  print_matrix();
}


void *worker(void* coord) {
  // calculates each matrix element in the coordinate received
  coordenate *c = ((coordenate *)coord);
  sleep(rand() % 10 + 1);
  pthread_mutex_lock(&pthread_next_request);
  C[c->row][c->column] = 0;
  for (int i = 0; i < K; i++) {
    C[c->row][c->column] += A[c->row][i] * B[i][c->column];
  }

  pthread_mutex_unlock(&pthread_next_request);


  pthread_exit(NULL);
}

void *show_matrix() {
  // prints the status of the matrix in each second, along a period of 10 seconds
  for (int i = 0; i < 10; i++) {
    printf("matrix after %d seconds\n", i);
    print_matrix();
    sleep(1);
  }
  pthread_exit(NULL);
}
