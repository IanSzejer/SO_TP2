#include <stdint.h>
#include "clock.h"
#include "time.h"
#include <videoD.h>
#include <keyboard.h>
#include <lib.h>
#include "memoryDriver.h"
#include "videoColors.h"

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define RED 4


extern void infoReg(char ** buf);

typedef uint64_t (*SysCallR)(uint64_t, uint64_t, uint64_t, uint64_t); // defino un puntero a funcion SysCallR

static long read(unsigned int fd, char * buf, uint64_t count); //deberia ser lo mismo que size_t
static long write(unsigned int fd,char * buf, uint64_t count, char color);
static void clear();
static int splitScreen(int screens, int screen);
static int changeScreen(int screen);
static int getChar(unsigned int ascii);
static void getTime(char * buf);
static long timerTick(void (*f)());
static void getDate(char * buf);



static SysCallR sysCalls[255] = {(SysCallR) &read, (SysCallR) &write, (SysCallR) &clear, (SysCallR) &splitScreen, (SysCallR) &changeScreen, (SysCallR)&getChar,(SysCallR)&ncClearLine,(SysCallR)&getTime, (SysCallR)&timerTick, (SysCallR)&set_kb_target, (SysCallR)&getDate, (SysCallR) &getRegs,(SysCallR) &malloc,(SysCallR) &free,(SysCallR) &memState}; 

uint64_t sysCallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t rax) {
    SysCallR sysCall = sysCalls[rax]; // sysCalls es un arreglo de punteros a funcion, me guardo la funcion que corresponde con el valor de rax
    if (sysCall != 0) // si no se encuentra en la lista, al estar inicializado con ceros el arreglo, me devuelve cero
        return sysCall(rdi, rsi, rdx, rcx);
    return 0;
}

static void clear() {
    ncClear();
}

static long timerTick(void (*f)()) {
    set_routine(f);
    return ticks_elapsed();
}

static long write(unsigned int fd,char * buf, uint64_t count, char color) {
    if (buf == NULL)
        return -1;
    if (fd == STDERR)
        color = RED;
    else if (fd != STDOUT) {
        return -1;
    }


    int i;
    for (i =0 ; buf[i] && i <count ; i++) {
        switch(buf[i]) {
            case '\n':
                ncNewline();
            break;
            case '\b':
                 ncDelete();
            break;
            default:
                    ncPrintChar(buf[i], color);
        }
    }
    return i > 0 ? i : -1;
}

static long read(unsigned int fd, char * buf, uint64_t count) {
    if (fd != 0)
        return -1; // solo acepta teclado
    return readFromKeyboard(buf, count,1);
}

static int getChar(unsigned int ascii) {
    int a;
         for(int i=0 ; i<1 ; i++) {
          a = kb_read();
          if (a == ALT)
            overwriteRegs();
          if (ascii && !PRINTABLE(a))
               i--;
     }
     return a;
}

static int splitScreen(int screens, int screen) {
    return ncSplitConsole(screens, screen);
}
static int changeScreen(int screen) {
    return ncChangeScreen(screen);
}

static int numToStr(int num, char * str) {
    if (num == 0) {
        str[0] = '0';
        return 1;
    }
	int neg=0;
	if (num<0) {
		str[0]='-';
		str++;
		num=num*(-1);
		neg=1;
	}	
	int i, rem, n, len = 0;
	n = num;
	while (n != 0) {
	  len++;
	  n /= 10;
	}
	for (i = 0; i < len; i++) {
	  rem = num % 10;
	  num = num / 10;
	  str[len - (i + 1)] = rem + '0';
	}
	return len+neg;
}


static void getTime(char * buf) {
    buf[0] = buf[3] = buf[6] = '0';
    buf[2] = buf[5] = ':';
    int h = hours();
    if (h<10)
        numToStr(h,&buf[1]);
    else numToStr(h,&buf[0]);

    int m = minutes();

    if (m<10)
        numToStr(m,&buf[4]);
    else numToStr(m,&buf[3]);

    int s = seconds();

    if (s<10)
        numToStr(s,&buf[7]);
    else numToStr(s,&buf[6]);
    buf[8] = 0;
}

static void getDate(char * buf) {
    buf[0] = buf[3] = '0';
    buf[2] = buf[5] = '/';

    int m = month();

    if (m<10)
        numToStr(m,&buf[1]);
    else numToStr(m,&buf[0]);

    int d = day();

    if (d<10)
        numToStr(d,&buf[4]);
    else numToStr(d,&buf[3]);

    int y = year();
    numToStr(y,&buf[6]);
    buf[8] = 0;
}

static void *malloc(size_t size){
    return mallocFun(size);
}

static void free(void *ptr){
    freeFun(ptr);
}

static void memState(){
    char buf[MAX_STR_LENGTH];
    consult(buf);
    write(STDOUT,buf,MAX_STR_LENGTH,WHITE);
}