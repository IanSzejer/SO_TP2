#ifndef _SUDOKU_H_
#define _SUDOKU_H_

#include "stdinout.h"
#include "colors.h"

#define LENGTH 9
#define OUT_OF_BOUNDS -1
#define PRED_VALUE -2
#define INVALID_INPUT -3
#define FINISHED_SUDOKU -4
void initSudoku();

// retorna -1 si la entrada es invalida
int sudokuInput(char a, int b, int value);

void initSudoku();

void printBoard();

void solveSudoku();


#endif