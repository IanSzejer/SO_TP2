#include "shell.h"
#include "stdinout.h"
#include "sudoku.h"
#include "colors.h"
#include "hangman.h"
#include "stopwatch.h"
//0x103CC4

typedef struct {
   char  buffer[25];
   char msg[50];
   int  bSize;
} BufT;

static BufT buffers[4];

void setup();
void multipleWindows();
void manageSudoku(BufT * buf);
void writeBuf(int win);
void manageStopwatch(BufT * buf);
void manageTime(char * buf);
void processInput(int window);
void manageHangman(BufT * buf);
void alwaysUpdate();


static char timeBuf[14] = {0};

static char msgBuf[64] = {0};
static char aux[10] = {0};

static int window = 0;


static const int windowChange[4][4] = {{2,2,1,1}, {3,3,0,0},{0,0,3,3},{1,1,2,2}};
static int kbPress = 0;

void multipleWindows() {
    set_kb_target(&kbPress);
    kbPress = 0;
    window = 0;
    split_screen(1,0);
    console_clear();
    split_screen(4,0);
    setup();
    while(1) {
        set_screen(window);
        if (kbPress) {
            if (ARROW(kbPress)) {
                window = windowChange[window][kbPress-501];
            }
            else {
                switch(kbPress) {
                    case '\n':
                        processInput(window);
                    break;
                    case '\b':
                    if (buffers[window].bSize>0) {
                        buffers[window].buffer[buffers[window].bSize--] = 0;
                        putChar('\b');
                    }
                    break;
                    case ESC:
                        kbPress = 0;
                        buffers[window].buffer[0] = 0;
                        return;
                    break;
                    default:
                        if (window != 1 && PRINTABLE(kbPress))
                            writeBuf(window);
                }
            }
            kbPress = 0;
        }
        alwaysUpdate();
    }   
}

void processInput(int window) {
    BufT * buf = &buffers[window];
    switch(window) {
        case 0: manageSudoku(buf);
        break;
        case 2: manageHangman(buf); 
        break;
        case 3: manageStopwatch(buf);
        break;
    }
    buf->buffer[0] = 0;
    buf->bSize = 0;

    if (buf->msg[0]) {
        clear_line();
        print(buf->msg);
        buf->msg[0] = 0;
    }
}

void writeBuf(int win) {
    BufT * buf = &buffers[window];
    buf->buffer[buf->bSize++] = kbPress;
    buf->buffer[buf->bSize] = 0;
    clear_line();
    print(buf->buffer);
}

void alwaysUpdate() {
    for (int i=0 ; i<4 ; i++) {
        set_screen(i);
        switch(i) {
            case 0: 

            break;

            case 1:
                get_time(aux);
                if (strcmp(timeBuf,aux)) {
                    console_clear();
                    manageTime(aux);
                    strcpy(timeBuf, aux); 
                }
            break;

            case 2:
            
            break;

            case 3:
            if (stopWatchChanged()) {
                console_clear();
                printStopWatch();
                print(buffers[i].buffer);
            }
            break;
        }
    }
    set_screen(window);
}

void setup() {
    for (int i=0 ; i<4 ; i++) {
        set_screen(i);
        console_clear();
        switch(i) {
            case 0: 
               initSudoku();
                printBoard();
            break;

            case 1:
                get_time(timeBuf);
                manageTime(timeBuf);
            break;

            case 3:
                initStopWatch();
                printStopWatch();
            break;

            case 2:
                initHangman();
                printHangman();
            break;
        }
    }
    set_screen(window);
}

void manageTime(char * time) {
    for (int i=0 ; i<6; i++)
        putChar('\n');
        print("                ");
        printColor(time, BLUE | GREY_BG);
}

void manageStopwatch(BufT * buf) {
    if (strcmp(buf->buffer,"start")==0) {
        stopWatchStart();
    }
    else if (strcmp(buf->buffer,"stop")==0) {
        stopWatchStop();
    }
    else if (strcmp(buf->buffer,"reset")==0) {
        stopWatchStop();
        initStopWatch();
        console_clear();
        printStopWatch();
    }
    else {
        strcpy(buf->msg, "Entrada invalida");
    }
    clear_line();
}

void manageHangman(BufT * buf) {

    char * buffer = buf->buffer;
    
    if (strcmp(buffer,"reset")==0) {
        initHangman();
    }
    else if (strcmp(buffer,"solve")==0) {
        solveHangman();
    }
    else {
        int value = inputLetter(buffer);
        switch(value) {
            case NOT_LETTER: strcpy(buf->msg, "El caracter ingresado no es una letra valida");
            break;
            case MORE_THAN_1_LETTER: strcpy(buf->msg, "Se ingreso mas de una letra");
            break;
            case GAME_COMPLETED: strcpy(buf->msg, "Felicitaciones! Juego terminado");
            break;
            case LETTER_REPEATED: strcpy(buf->msg, "La letra ya fue usada");
            break;
            case GAME_LOST: strcpy(buf->msg, "Ha perdido el juego.");
            break;
            default: buf->msg[0]=0;
        }
    }
    console_clear();
    printHangman();
}


void manageSudoku(BufT * buf) {
    char * strings [3];
    char * b = buf ->buffer;

    clear_line();


    if (strcmp(b,"solve")==0) {
        solveSudoku();
        console_clear();
        printBoard();
    }

    else if (strcmp(b,"reset")==0) {
        initSudoku();
        console_clear();
        printBoard();
    }
    
    else if (split(b,' ', strings) != 3) {
        strcpy(msgBuf, "Entrada invalida");
    }

    else {
        int value = sudokuInput(strings[0][0], charToDigit(strings[1][0]), charToDigit(strings[2][0]));

        switch(value) {
            case OUT_OF_BOUNDS: strcpy(buf->msg, "Valores fuera de rango");
            break;
            case PRED_VALUE: strcpy(buf->msg, "No se pueden modificar los verdes");
            break;
            case INVALID_INPUT: strcpy(buf->msg, "Jugada invalida");
            break;
            case FINISHED_SUDOKU: strcpy(buf->msg, "Felicitaciones!");
            break;
            default: buf->msg[0]=0;
        }
        console_clear();
        printBoard();
    }
    
    

}