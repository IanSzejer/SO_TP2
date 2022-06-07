#include "keyboard.h"

#define MAX_SIZE 150
#define CAPSLOCK 0x3A
#define LSHIFT 0x2A
#define RSHIFT 0x36
#define BACKSPACE 0x0E
#define ESC 0x01
#define CTRL 0x1D
#define ESC_ASCII 27
#define EOF -1

static unsigned int index_buffer = 0;
static int mayusFlag = 0;
static char keyChar;
static int shiftFlag = 0;
static int controlFlag = 0;

// este mapa esta basado en 'Keyboard Scan Codes'
static char kbd_US[128][2] =
    {
        {0, 0},
        {0, 0},
        {'1', '!'},
        {'2', '@'},
        {'3', '#'},
        {'4', '$'},
        {'5', '$'},
        {'6', '^'},
        {'7', '&'},
        {'8', '*'},
        {'9', '('},
        {'0', ')'},
        {'-', '_'},
        {'=', '+'},
        {'\b', '\b'},
        {'\t', '\t'},
        {'q', 'Q'},
        {'w', 'W'},
        {'e', 'E'},
        {'r', 'R'},
        {'t', 'T'},
        {'y', 'Y'},
        {'u', 'U'},
        {'i', 'I'},
        {'o', 'O'},
        {'p', 'P'},
        {'[', '{'},
        {']', '}'},
        {'\n', '\n'},
        {0, 0},
        {'a', 'A'},
        {'s', 'S'},
        {'d', 'D'},
        {'f', 'F'},
        {'g', 'G'},
        {'h', 'H'},
        {'j', 'J'},
        {'k', 'K'},
        {'l', 'L'},
        {';', ':'},
        {'\'', '\"'},
        {'`', '~'},
        {0, 0},
        {'\\', '|'},
        {'z', 'Z'},
        {'x', 'X'},
        {'c', 'C'},
        {'v', 'V'},
        {'b', 'B'},
        {'n', 'N'},
        {'m', 'M'},
        {',', '<'},
        {'.', '>'},
        {'/', '?'},
        {0, 0},
        {0, 0},
        {0, 0},
        {' ', ' '},
        {0, 0}};

static int isMayus(char keyCode);
static int isShift(char keyCode);
static int isControl(char keyCode);
static int isEscape(char keyCode);

static int keyPressed(char keyCode);
static char buffer[MAX_SIZE]={0};
void keyboard_handler() {
    char keyCode = getKbCode();

    if (keyPressed(keyCode)) {

        // Si es la tecla 'CAPS LOCK' prendemos el flag y lo volvemos a apagar unicamente cuando se presione de nuevo.
        // Retornamos pues no es una tecla que debemos imprimir en pantalla
        if (isMayus(keyCode)) {
            mayusFlag = (mayusFlag == 1) ? 0 : 1;
            return;
        }

        // Si es la tecla 'RIGHT SHIFT' o 'LEFT SHIFT' prendemos el flag de la tecla.
        // Retornamos pues no es una tecla que debemos imprimir en pantalla
        if (isShift(keyCode)) {
            shiftFlag = 1;
            return;
        }

        if (isEscape(keyCode)) {
            keyChar = ESC_ASCII;
            return;
        }

        // Si la tecla es 'CTRL' guardamos los registros que se hayan guardado
        if (isControl(keyCode)) {
            controlFlag = 1;
            return;
        }

        if ((mayusFlag + shiftFlag) % 2 == 0) {
            
            keyChar = kbd_US[keyCode][0];
        } else {
            
            keyChar = kbd_US[keyCode][1];
        }

        switch (keyChar) {
            case '\b':
                if (index_buffer > 0) {
                    //ncPrintChar(keyChar);
                    ncBackspace();
                    index_buffer--;
                    buffer[index_buffer] = 0;
                }
                break;

            case 'r':
            case 'R':
                if(controlFlag){
                  //uint64_t* copy = get_saved_registers();
                  //write_registers(copy);
                }

            case 'd':
            case 'D':
                if(controlFlag){
                  buffer[index_buffer++] = EOF;
                }

            case 'c':
            case 'C':
                if(controlFlag){
                    killProcess(getProcessRunning());
                }

            case 'z':
            case 'Z':

            default:
                if(!controlFlag){
                  buffer[index_buffer++] = keyChar;
                  ncPrintChar(keyChar);
                }
                break;
        }
    }
    else {
        if (keyCode == (LSHIFT - 0x80) || keyCode == (RSHIFT - 0x80)) {
            shiftFlag = 0;
        }else if (keyCode == (CTRL - 0x80)){
            controlFlag = 0;
        }
        keyChar = 0;
    }
}




static int keyPressed(char keyCode) {
    return (keyCode & 0b10000000) == 0b00000000;
}



char* kb_read() {
    return buffer;
}


int sizeBuffer() {
    return index_buffer;
}

void resetBuffer() {
    index_buffer = 0;
}
static int isMayus(char keyCode) {
    return keyCode == CAPSLOCK;
}

static int isShift(char keyCode) {
    return keyCode == LSHIFT || keyCode == RSHIFT;
}

static int isControl(char keyCode) {
    return keyCode == CTRL;
}

static int isEscape(char keyCode) {
    return keyCode == ESC;
}

/*
void overwriteRegs() {
     updateRegs(stackBackup);
}


void keyboardDriver(uint64_t * stack) {
     for (int i=0 ; i<REG_COUNT-1 ; i++)
          stackBackup[i] = stack[i];
     int c;
     while(keyboardActivated()) {
          c = getKbCode();
     }
     if (target && !(c & 0x80)) {
          *target = getAscii(c);
          if (*target == ALT)
               overwriteRegs();
     }
          

}
      	

void set_kb_target(int * var) {
     target = var;
}

int readFromKeyboard(char * buf, uint64_t count, int ascii) {
     //picMasterMask(0xFE);
     for(int i=0 ; i<count ; i++) {
          buf[i] = kb_read();
          pressedValue = buf[i];
          if (ascii && !PRINTABLE(buf[i]))
               i--;
     }
     return count;
}*/