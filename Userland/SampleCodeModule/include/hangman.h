#ifndef _HANGMAN_H_
#define _HANGMAN_H_

#include "stdinout.h"

#define NOT_LETTER 1
#define MORE_THAN_1_LETTER 2
#define GAME_COMPLETED 3
#define LETTER_REPEATED 4
#define GAME_LOST 5

void restartHagnman();
void initHangman();
void printHangman();
void solveHangman();
int inputLetter(char* shellBuffer);

#endif