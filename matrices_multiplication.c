/*
 * Multi-threaded matrices multiplication
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int **result;
int *row_status;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct m_data {
  pthread_mutex_t *lock;
  int thread;
  int rows; // Rows of the final matrix
  int columns; // Columns of the final matrix
  int **matrix1;
  int **matrix2;
};

// void *runner(void *param); // That's the thread!

void *row_calculator(void *param) {

  // Gets the number of the rows the thread will be working on
  struct m_data *details = param;

  int rows = details->rows;
  int columns = details->columns;
  int **matrix1 = details->matrix1;
  int **matrix2 = details->matrix2;

  for (int i = 0; i < rows; i++) {

    //printf("Thread: %d, row: %d\n", details->thread, i);

    pthread_mutex_lock(details->lock);
    if (row_status[i] == 1) {
      i++;
      //printf("Thread: %d quits on row: %d\n", details->thread, i);
      pthread_mutex_unlock(details->lock);
      continue;
    }
    else if (row_status[i] == 0) {
      row_status[i] = 1;
      //printf("Thread: %d proceeds on row: %d\n", details->thread, i);
    }
    else {
      fprintf(stderr, "Wrong value in row status.");
      exit(0);
    }
    pthread_mutex_unlock(details->lock);

    //printf("-> Row locks for thread %d on row %d: current: %d, next: %d\n", details->thread, i, row_status[i], row_status[i + 1]);

    int sum[rows];

    for (int j = 0; j < rows; j++) { // J is the number of the column from the second matrix
      int cell_sum = 0;

      bool value = false;

      for (int k = 0; k < columns; k++) { // K is the index of the column from the first matrix
        value = value || (matrix1[i][k] && matrix2[k][j]);

        if (value)
          break;
      }
      sum[j] = value;
    }

    for (int l = 0; l < rows; l++) {
      result[i][l] = sum[l];
    }

    //printf("\n");
  }
  // Exit the thread
  pthread_exit(0);
}

void multiply_matrices(int **matrix1, int **matrix2, int rows, int columns, int threads) {

  pthread_t *tids = malloc(sizeof(pthread_t) * threads);
  pthread_mutex_t lock;
  struct m_data *details = (struct m_data *) malloc(sizeof(struct m_data));

  pthread_mutex_init(&lock, NULL);
  details->rows = rows;
  details->lock = &lock;
  details->columns = columns;
  details->matrix1 = matrix1;
  details->matrix2 = matrix2;

  for (int i = 0; i < threads; i++) {

    // Store number of thread for use later.
    details->thread = i;

    // Creates the thread, passing it the number of the row as a parameter
    pthread_t tid;        // Thread's ID
    pthread_attr_t attr;  // Thread's attributes

    // Adds the thread to the thread_pool
    tids[i] = tid;

    // Sets the default atttributes for the thread
    pthread_attr_init(&attr);

    // Creates the thread
    pthread_create(&tid, &attr, row_calculator, details); // i -> number of row

    // Waits for all the threads to complete their calculations before continuing
    //pthread_join(tid, NULL);
  }

  for (int i = 0; i < threads; i++) {
    pthread_join(tids[i], NULL);
  }
}

void print_matrix(int **matrix, int rows, int columns) {

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      printf("%d  ", matrix[i][j]);
    }
    printf("\n");
  }
}

void tests() {

  int rows_m1 = 2;
  int columns_m1 = 3;

  int rows_m2 = 3;
  int columns_m2 = 2;

  int **m1 = malloc(rows_m1 * sizeof * m1);
  int **m2 = malloc(rows_m2 * sizeof * m2);
  result = malloc(rows_m1 * sizeof * m1);
  row_status = malloc(rows_m1 * rows_m2 * sizeof m1);

  for(int i = 0; i < rows_m1; i++)
    m1[i] = malloc(columns_m1 * sizeof *m1[i]);

  for(int i = 0; i < rows_m2; i++)
    m2[i] = malloc(columns_m2 * sizeof *m2[i]);

  for(int i = 0; i < rows_m1; i++)
    result[i] = malloc(columns_m2 * sizeof *m1[i]);

  for(int i = 0; i < rows_m1; i++) {
    row_status[i] = 0;
  }

  m1[0][0] = 2;
  m1[0][1] = 1;
  m1[0][2] = 3;
  m1[1][0] = -1;
  m1[1][1] = 2;
  m1[1][2] = 4;

  m2[0][0] = 1;
  m2[0][1] = 3;
  m2[1][0] = 2;
  m2[1][1] = -2;
  m2[2][0] = -1;
  m2[2][1] = 4;

  printf("Matrix 1:\n");
  print_matrix(m1, 2, 3);
  printf("\nMatrix 2:\n");
  print_matrix(m2, 3, 2);
  printf("\n");

  multiply_matrices(m1, m2, 2, 3, 2); // Make sure to provide the number of rows and columns of the first matrix!

  for (int i = 0 ; i < rows_m1; i++) {
    free(m1[i]);
  }
  free(m1);

  for (int i = 0 ; i < rows_m2; i++) {
    free(m2[i]);
  }
  free(m2);

  printf("\nFinal result:\n");
  print_matrix(result, 2, 2);

}

void generate_random_matrix(int **matrix, int columns, int rows) {

  for (int i = 0; i < columns; i++) {
    for (int j = 0; j < rows; j++) {
      matrix[i][j] = arc4random()%2;
    }
  }

}

int main(int argc, char *argv[]) {

  if (argc != 4) {
    fprintf(stderr, "usage: rows, columns, threads, argc: %d\n", argc);
    exit(1);
  }

  if (argv[1] < 0 || argv[2] < 0 || argv[3] < 0) {
    fprintf(stderr, "invalid values provided");
    exit(1);
  }

  int rows_m1 = atoi(argv[1]),
      columns_m1 = atoi(argv[2]),
      threads = atoi(argv[3]),
      rows_m2 = columns_m1,
      columns_m2 = rows_m1;

  // Creates matrices we'll be working on
  int **m1 = malloc(rows_m1 * sizeof * m1);
  int **m2 = malloc(rows_m2 * sizeof * m2);

  // Allocates and initializes global
  result = malloc(rows_m1 * sizeof * m1);
  row_status = malloc(rows_m1 * sizeof m1);

  for (int i = 0; i < rows_m1; i++)
    m1[i] = malloc(columns_m1 * sizeof *m1[i]);

  for (int j = 0; j < rows_m2; j++)
    m2[j] = malloc(columns_m2 * sizeof *m2[j]);

  for (int k = 0; k < rows_m1; k++)
    result[k] = malloc(columns_m2 * sizeof(int));

  for (int l = 0; l < rows_m1; l++)
    row_status[l] = 0;

  generate_random_matrix(m1, rows_m1, columns_m1);
  generate_random_matrix(m2, rows_m2, columns_m2);

  printf("Matrix 1:\n");
  print_matrix(m1, rows_m1, columns_m1);
  printf("\nMatrix 2:\n");
  print_matrix(m2, rows_m2, columns_m2);

  printf("\nResult:\n");
  multiply_matrices(m1, m2, rows_m1, columns_m1, 2); // Make sure to provide the number of rows and columns of the first matrix!
  print_matrix(result, rows_m1, columns_m2);

  return 0;
}
