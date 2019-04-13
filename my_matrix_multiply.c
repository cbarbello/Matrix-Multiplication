//
//  my_matrix_multiply.c
//  Lab 3 - CS170 Winter 2015
//  Created by Casey Barbello on 1/12/15.
//

#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h>
#include "c-timer.h"

#define ARGS "a:b:t:"

struct matrix
{
  int numRows;
  int numCols;
  double *data;
};

struct arg_struct
{
  int id;
  int index;
  int rowsToCompute;
  int matrixC_index;
  struct matrix *A;
  struct matrix *B;
  struct matrix *C;
};

void *matrixMultiply(void *arg)
{
  struct arg_struct *my_args = (struct arg_struct *)arg;
  int id = my_args->id;
  int index = my_args->index;
  int rowsToCompute = my_args->rowsToCompute;
  int matrixC_index = my_args->matrixC_index;
  struct matrix *matrixA = my_args->A;
  struct matrix *matrixB = my_args->B;
  struct matrix *matrixC = my_args->C;
  double sum;
  int iteratorB = 0;

  for(int i = 0; i < matrixC->numCols * rowsToCompute; i++){
    // this is for the case where a thread needs to work on more than one row.
    // we check if we're on to the next column and then reset our iteratorB and adjust the index.
    if (i % matrixC->numCols == 0 && i > 0){
      iteratorB = 0;
      index += matrixA->numCols;
    }
    sum = 0.0;
    int rowNum = 0;
    // compute the sum
    for(int j = index; j < matrixA->numCols + index; j++){
      sum += matrixA->data[j] * matrixB->data[matrixB->numCols*rowNum + iteratorB];
      rowNum++;
    }
    iteratorB++;
    matrixC->data[matrixC_index] = sum;
    matrixC_index += 1;
  }
  return((void *) my_args);
}


int main(int argc, char **argv)
{
  FILE *fa, *fb;
  int numRowsA, numColsA, numRowsB, numColsB, numThreads;

  if(argc != 7) {
    printf("usage: my_matrix_multiply -a a_matrix_file.txt -b b_matrix_file.txt -t thread_count\n");
    exit(0);
  }
  int opt;
  while ((opt = getopt(argc, argv, ARGS)) != -1) {
    switch (opt) {
      case 'a':
        fa = fopen(optarg, "r");
        break;
      case 'b':
        fb = fopen(optarg, "r");
        break;
      case 't':
        numThreads = atoi(optarg);
        break;
      default:
        fprintf(stderr, "usage: my_matrix_multiply -a a_matrix_file.txt -b b_matrix_file.txt -t thread_count\n");
        exit(0);
    }
  }

  if (fa == NULL || fb == NULL){
    fprintf(stderr, "error: could not open file.\n");
    exit(0);
  }

  if (numThreads < 1){
    fprintf(stderr, "error: number of threads must be greater than 0.\n");
    exit(0);
  }

  if(fscanf(fa, "%d %d", &numRowsA, &numColsA) < 2) {
    fprintf(stderr, "error: input file for matrix A is formatted improperly.\n");
    exit(0);
  }
  if(fscanf(fb, "%d %d", &numRowsB, &numColsB) < 2) {
    fprintf(stderr, "error: input file for matrix B is formatted improperly.\n");
    exit(0);
  }

  // check if matrix multiplication is possible
  if(numColsA != numRowsB) {
    fprintf(stderr, "error: Matrix multiplication not possible with given matrices.\n");
    exit(0);
  }

  double *arrayA = (double *)malloc((numRowsA * numColsA) * sizeof(double));
  double *arrayB = (double *)malloc((numRowsB * numColsB) * sizeof(double));
  double *arrayC = (double *)malloc((numRowsA * numColsB) * sizeof(double));
  char line[100];
  int i = 0;
  int j = 0;

  while(fgets(line, 100, fa)) {
    if(sscanf(line, "%lf", &arrayA[i]) == 1){
      i++;
    }
  }
  if(i < numRowsA * numColsA){
    fprintf(stderr, "error: input file for matrix A formatted improperly.\n");
    exit(0);
  }

  while(fgets(line, 100, fb)){
    if(sscanf(line, "%lf", &arrayB[j]) == 1) {
      j++;
    }
  }
  if(j < numRowsB * numColsB){
    fprintf(stderr, "error: input file for matrix B formatted improperly.\n");
    exit(0);
  }
  fclose(fa);
  fclose(fb);

  struct matrix *matrixA = (struct matrix *)malloc(sizeof(struct matrix));
  struct matrix *matrixB = (struct matrix *)malloc(sizeof(struct matrix));
  struct matrix *matrixC = (struct matrix *)malloc(sizeof(struct matrix));
  matrixA->numRows = numRowsA;
  matrixA->numCols = numColsA;
  matrixA->data = arrayA;
  matrixB->numRows = numRowsB;
  matrixB->numCols = numColsB;
  matrixB->data = arrayB;
  matrixC->numRows = numRowsA;
  matrixC->numCols = numColsB;
  matrixC->data = arrayC;

  pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t)*numThreads);
  struct arg_struct *args;

  // make adjustment if too many threads are specified
  if(numThreads > numRowsA){
    numThreads = numRowsA;
  }
  // divide up the rows evenly amongst the threads and track the remainder
  int remainderRows = numRowsA % numThreads;
  int threadRows = (numRowsA - remainderRows)/numThreads;
  int index = 0;
  int matrixC_index = 0;
  int err;

  double timer = CTimer();

  fflush(stdout);
  for(int i = 0; i < numThreads; i++){
    args = (struct arg_struct *)malloc(sizeof(struct arg_struct));
    args->id = i;
    args->A = matrixA;
    args->B = matrixB;
    args->C = matrixC;
    args->index = index;
    args->matrixC_index = matrixC_index;

    // distribute the remainder rows amongst the first n threads
    if(i < remainderRows){
      args->rowsToCompute = threadRows + 1;
    }
    else{
      args->rowsToCompute = threadRows;
    }
    index += numColsA * args->rowsToCompute;
    matrixC_index += matrixC->numCols * args->rowsToCompute;
    err = pthread_create(&(threads[i]), NULL, matrixMultiply, (void *)args);
  }
  
  for(int i = 0; i < numThreads; i++){
    fflush(stdout);
    err = pthread_join(threads[i], (void **)&args);
  }

  double timePassed = CTimer() - timer;/////////

  int it = 0;
  printf("%d %d\n", args->C->numRows, args->C->numCols);

  for(int i = 0; i < args->C->numRows * args->C->numCols; i++){
    if(i % args->C->numCols == 0){
      printf("# Row %d\n", it);
      it++;
    }
    printf("%lf\n", args->C->data[i]);
  }

  free(arrayA);
  free(arrayB);
  free(arrayC);
  free(matrixA);
  free(matrixB);
  free(matrixC);
  free(threads);
  free(args);

  printf("Elapsed time: %f\n", timePassed);

  return(0);
}

// commands (for easy pasting into the shell)
// g++ my_matrix_multiply.c -o my_matrix_multiply
// ./my_matrix_multiply -a a_matrix_file.txt -b b_matrix_file.txt -r 1
// g++ my_matrix_multiply.c c-timer.c -o my_matrix_multiply
