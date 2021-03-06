#ifndef _STDINOUT_H_
#define _STDINOUT_H_


#include<stddef.h>
typedef __UINT64_TYPE__ uint64_t;
// Salida/Entrada
#define STDIN 0
#define STDOUT 1

#include "colors.h"
#include "chardefs.h"


extern int system_read(unsigned int fd, char* buffer, unsigned int buffercount);    
extern int system_write(unsigned int fd, char*buffer, unsigned int buffersize, char color);
extern void console_clear();  
extern int split_screen(int screens, int screen); // retorna 1 si la seleccion es valida. -1 si no.
extern int set_screen(int screen);  // retorna 1 si la seleccion es valida. -1 si no
extern int get_char(int ascii);
extern void clear_line();
extern long timer_tick(void (*f)());
extern void get_date(char * buf);
extern void get_regs(char ** buf);
extern void get_memory(unsigned int * p);
extern void *malloc(size_t size);
extern void free(void *ptr);
extern void mem_state();
extern uint64_t new_process(void *(*funcion)(void *), void *argv, int argc[2],char* name);
extern void end_process(uint64_t pid);
extern void kill(uint64_t pid);
extern int getPid();
extern int nice(uint64_t pid,uint64_t priority);
extern void change_state(uint64_t pid, int status);
extern void changeProcesses();
extern int createSemaphore(char* name,uint64_t value);
extern int openSemaphore(char* name,uint64_t value);
extern int closeSemaphore(char* semName);
extern void getSemaphores();
extern int waitSem(char* semaphore);
extern int postSem(char* semaphore);
extern int createPipe(int pipeFd[2]);
extern void openPipe(void *ptr);
extern void get_pipes();
extern int get_pid_sys();
extern int dup2(uint64_t oldFd,uint64_t newFd);
extern int get_all_processes();
extern void my_yield();
extern void exit();
extern void waitProcess(uint64_t pid);
extern void* getSharedMemoryBlock(uint64_t id);

void print(char * string);
int strlength(char * string);
void printColor(char * string, char color);
char* itoa(int num, char* str, int base);
int power(int num, int p);
int strToInt(char * buff, int len);
int isDigit(char c);
int charToDigit(char c);
char toLower(char c);
char toUpper(char c);
int getCharSys();
int putChar(char c);
int putCharColor(char c, char color);
int strcmp(char * s1, char * s2);
char * strcpy(char* destination, char* source);
int getRealChar();
int atoi(char * s);
int atoi_base(const char *str, int str_base);
char intToChar(unsigned int num);
int split(char * buf,char c,char * target[]);
int ticks();
long secondsElapsed();
int strcat(char * target, char * source);
int numToStr(int num, char* str, int base);
uint64_t uintToBase(uint64_t value, char *buffer, uint64_t base);
int scanf(char* buf);
#endif