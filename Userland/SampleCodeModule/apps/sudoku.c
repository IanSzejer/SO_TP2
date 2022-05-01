#include "sudoku.h"


#define LINE " +-------------------+\n"
#define SPACES "        "

static int board[LENGTH][LENGTH] = 
        {{3, 0, 6, 5, 0, 8, 4, 0, 0}, 
         {5, 2, 0, 0, 0, 0, 0, 0, 0}, 
         {0, 8, 7, 0, 0, 0, 0, 3, 1}, 
         {0, 0, 3, 0, 1, 0, 0, 8, 0}, 
         {9, 0, 0, 8, 6, 3, 0, 0, 5}, 
         {0, 5, 0, 0, 9, 0, 6, 0, 0}, 
         {1, 3, 0, 0, 0, 0, 2, 5, 0}, 
         {0, 0, 0, 0, 0, 0, 0, 7, 4}, 
         {0, 0, 5, 2, 0, 6, 3, 0, 0} };

static const int boardTemplate[LENGTH][LENGTH] = 
        {{3, 0, 6, 5, 0, 8, 4, 0, 0}, 
         {5, 2, 0, 0, 0, 0, 0, 0, 0}, 
         {0, 8, 7, 0, 0, 0, 0, 3, 1}, 
         {0, 0, 3, 0, 1, 0, 0, 8, 0}, 
         {9, 0, 0, 8, 6, 3, 0, 0, 5}, 
         {0, 5, 0, 0, 9, 0, 6, 0, 0}, 
         {1, 3, 0, 0, 0, 0, 2, 5, 0}, 
         {0, 0, 0, 0, 0, 0, 0, 7, 4}, 
         {0, 0, 5, 2, 0, 6, 3, 0, 0} };


static int cursor[2] = {0};

void moveCursor(int x, int y) {
    if (cursor[0]+x >= LENGTH)
        cursor[0] = LENGTH-1;
    else
        cursor[0] += x;

    if (cursor[1]+y >= LENGTH)
        cursor[1] = LENGTH-1;
    else
        cursor[1] += y;
}

int possibleMove(int row, int col, int num) {

    for (int i = 0; i <= 8; i++)
        if (board[row][i] == num)
            return 0;

    for (int i = 0; i <= 8; i++)
        if (board[i][col] == num)
            return 0;

    int startRow = row - row % 3,
            startCol = col - col % 3;
   
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i + startRow][j +
                            startCol] == num)
                return 0;
 
    return 1;
}

int solve(int row, int col)
{

    if (row == LENGTH - 1 && col == LENGTH)
        return 1;
 
    if (col == LENGTH) {
        row++;
        col = 0;
    }
   
    if (board[row][col] > 0)
        return solve(row, col + 1);
 
    for (int num = 1; num <= LENGTH; num++) {

        if (possibleMove(row, col, num)) {
            board[row][col] = num; 
   
            if (solve(row, col + 1))
                return 1;
        }
       
        board[row][col] = 0;
    }
    return 0;
}

static void cleanArray(int checked[9]) {
    for (int i=0 ; i<9 ; i++)
        checked[i] = 0;
}

static int checkSubMatrix(int row, int col) {
    int checked[9] = {0};
     for (int i=row ; i<=row+3 ; i++) {
            for (int j = col ; j<col+3 ; j++) {
                if (checked[board[i][j]])
                    return 0;
                checked[board[i][j]] = 1;
            }
        }
    return 1;
}

int isSolved() { 
    int checked[9] = {0};
    for (int i=0 ; i<=8 ; i++) {
        for (int j = 0; j <= 8; j++) {
            if (checked[board[i][j]])
                return 0;
            checked[board[i][j]] = 1;
        }
    }

    cleanArray(checked); 

        for (int i=0 ; i<=8 ; i++) {
        for (int j = 0; j <= 8; j++) {
            if (checked[board[j][i]])
                return 0;
            checked[board[j][i]] = 1;
        }
    }
    for (int i=0 ; i<9 ; i+=3) {
        for (int j=0 ; j<9 ; j+=3)
            if (!checkSubMatrix(i,j))
                return 0;
    }
       
    return 1;

}

void solveSudoku() {
    initSudoku();
    solve(0,0);
}

void printBoard() {
    print(SPACES);
    print(LINE);
    for (int i=0 ; i<LENGTH ; i++) {
        print(SPACES);
        putChar('A' + i);
        print("| ");
        for (int j=0 ; j<LENGTH ; j++) {
            char c = intToChar(board[i][j]);
            if (board[i][j] == 0) {
                putCharColor(' ', GREY | GREY_BG);
            }
            else if (board[i][j] == boardTemplate[i][j]) {
                putCharColor(c, GREEN);
            }
            else {
                putCharColor(c, GREY);
            }
            if (j!=LENGTH-1)
                putChar(' ');
        }
        print(" |");
        putChar('\n');
    }
    print(SPACES);
    print(LINE);
    print(SPACES);
    print("   1 2 3 4 5 6 7 8 9");
}

int sudokuInput(char a, int b, int value) {
        int x;
    if (a <= 'i' && a >= 'a')
        x =a -  'a';
    else {
       x  = a - 'A';
    }

    int y = b-1;
        if (x >= LENGTH || x < 0 || y >= LENGTH || y < 0)
        return OUT_OF_BOUNDS;

    if (boardTemplate[x][y]>0)
        return PRED_VALUE;
    int aux = board[x][y];
    board[x][y] = 0;

    if (possibleMove(x,y,value)) {
        board[x][y] = value;
        if (isSolved())
            return FINISHED_SUDOKU;
        return 0;
    }
    board[x][y] = aux;
    return INVALID_INPUT;
}

void initSudoku() {
    for (int i=0 ; i<LENGTH ; i++) {
        for (int j=0 ; j<LENGTH ; j++)
            board[i][j] = boardTemplate[i][j];
    }
}