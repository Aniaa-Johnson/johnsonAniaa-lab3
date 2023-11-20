#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab3.h"


int** read_board_from_file(char* filename){
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return NULL;
    }
    int** board = (int**)malloc(sizeof(int*) * ROW_SIZE);
    for (int i = 0; i < ROW_SIZE; i++) {
        board[i] = (int*)malloc(sizeof(int) * COL_SIZE);
    }

    for (int i = 0; i < ROW_SIZE; i++) {
        for (int j = 0; j < COL_SIZE; j++) {
            if (fscanf(fp, "%d,", &board[i][j]) != 1) {
                fprintf(stderr, "Error reading from file.\n");
                fclose(fp);

               
                for (int k = 0; k < ROW_SIZE; k++) {
                    free(board[k]);
                }
                free(board);
                return NULL;
            }
        }
    }
    fclose(fp);
    return board;
}

int is_board_valid(){
    pthread_t* tid;  /* the thread identifiers */
    pthread_attr_t attr;
    param_struct* parameter;
    
    // replace this comment with your code
     worker_validation = (int*)malloc(sizeof(int) * NUM_OF_THREADS);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        params[i].id = i;
        // Set values for starting row, starting column, ending row and ending column in params[i]
       
        pthread_create(&tid[i], &attr, validate, &params[i]);
    }

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (worker_validation[i] == 0) {
            free(tid);
            free(params);
            free(worker_validation);
            return 0;
        }
    }

    free(tid);
    free(params);
    free(worker_validation);
    return 1;
}

void* validate(void* param) {
    param_struct* my_params = (param_struct*)param;

    // Validate rows
    worker_validation[my_params->id] = is_valid(
        my_params->starting_row, 0, my_params->ending_row, COL_SIZE - 1, VALIDATE_ROW
    );

    if (!worker_validation[my_params->id]) {
        pthread_exit(NULL);
    }

    // Validate columns
    worker_validation[my_params->id] = is_valid(
        0, my_params->starting_col, ROW_SIZE - 1, my_params->ending_col, VALIDATE_COLUMN
    );

    if (!worker_validation[my_params->id]) {
        pthread_exit(NULL);
    }

    // Validate 3x3 subgrids
    worker_validation[my_params->id] = is_valid(
        my_params->starting_row, my_params->starting_col,
        my_params->ending_row, my_params->ending_col, VALIDATE_SUBGRID
    );

    pthread_exit(NULL);
}

int is_valid(int start_row, int start_col, int end_row, int end_col, int validation_type) {
    int seen[10] = {0};

    for (int i = start_row; i <= end_row; i++) {
        for (int j = start_col; j <= end_col; j++) {
            int digit;

            if (validation_type == VALIDATE_ROW) {
                digit = sudoku_board[i][j];
            } else if (validation_type == VALIDATE_COLUMN) {
                digit = sudoku_board[j][i];
            } else if (validation_type == VALIDATE_SUBGRID) {
                digit = sudoku_board[start_row + i % 3][start_col + j % 3];
            }

            if (digit < 1 || digit > 9 || seen[digit]) {
                return 0;
            }

            seen[digit] = 1;
        }
    }

    return 1;
}

    
