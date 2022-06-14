#include <stdint.h>
#include "clock.h"
#include "time.h"
#include <videoD.h>
#include <keyboard.h>
#include <lib.h>
#include "memoryDriver.h"
#include "videoColors.h"
#include "scheduler.h"
#include "pipeManager.h"
#include "sem.h"
#include "interrupts.h"
#include "sharedMemory.h"
#define STDIN 0
#define STDOUT 1
#define STDERR 2

int mayus = 0;

extern void infoReg(char **buf);

typedef uint64_t (*SysCallR)(uint64_t, uint64_t, uint64_t, uint64_t); // defino un puntero a funcion SysCallR

static int read(unsigned int fd, char *buf, int count); // deberia ser lo mismo que size_t
static long write(uint64_t fd, char *buf, int count, int color);
static void clear();
static int getCharSys(unsigned int ascii);
static void getTime(char *buf);
static long timerTick(void (*f)());
static void *malloc(size_t size);
static void free(void *ptr);
static void memState();
static uint64_t newProcess(void *(*funcion)(void *), void *argv, int argc[2],char* name);
static void endProcess(uint64_t pid);
static void kill(uint64_t pid);
static void getAllProcesses();
static int nice(uint64_t pid,uint64_t priority);
static void changeState(uint64_t pid, int status);
static void changeProcesses();
static int createSemaphore(char* name,uint64_t value);
static int openSemaphore(char* name,uint64_t value);
static int closeSemaphore(char* semName);
static void getSemaphores();
static int waitSem(char* semaphore);
static int postSem(char* semaphore);
static int createPipe(int pipeFd[2]);
static void openPipe(void *ptr);
static void getPipes();
static int getPidSys();
static int dup2(uint64_t oldFd, uint64_t newFd);
static void myYield();
static void exit();
static void wait(uint64_t pid);
static void* getSharedMemoryBlock(uint64_t id);

static SysCallR sysCalls[255] = {(SysCallR)&read, (SysCallR)&write, (SysCallR)&clear, (SysCallR)&getCharSys, (SysCallR)&getTime,
                                 (SysCallR)&timerTick,
                                 (SysCallR)&wait, (SysCallR)&getRegs, (SysCallR)&malloc, (SysCallR)&free,
                                 (SysCallR)&memState, (SysCallR)&newProcess, (SysCallR)&endProcess, (SysCallR)&kill, (SysCallR)&getAllProcesses, (SysCallR)&nice,
                                 (SysCallR)&changeState, (SysCallR)&changeProcesses, (SysCallR)&createSemaphore, (SysCallR)&openSemaphore,
                                 (SysCallR)&closeSemaphore, (SysCallR)&getSemaphores,
                                 (SysCallR)&waitSem, (SysCallR)&postSem, (SysCallR)&createPipe, (SysCallR)&openPipe,
                                 (SysCallR)&getPipes,(SysCallR)&getPidSys,(SysCallR)&dup2, (SysCallR)&myYield, (SysCallR)&exit,(SysCallR)&getSharedMemoryBlock};

uint64_t sysCallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t rax)
{
    SysCallR sysCall = sysCalls[rax]; // sysCalls es un arreglo de punteros a funcion, me guardo la funcion que corresponde con el valor de rax
    if (sysCall != 0)                 // si no se encuentra en la lista, al estar inicializado con ceros el arreglo, me devuelve cero
        return sysCall(rdi, rsi, rdx, rcx);
    return 0;
}

static void clear()
{
    ncClear();
}

static long timerTick(void (*f)())
{
    set_routine(f);
    return ticks_elapsed();
}

static long write(uint64_t fd, char *buf, int count, int color)
{
    uint64_t ref;
    if (buf == NULL)
        return -1;
    if (fd == STDERR)
        color = RED;
    else if((ref=getFdRef(fd))>=100 && ref<122 && ref%2==1){
        writeInPipe(ref,getProcessRunning(),strlen(buf,count),buf);
        return strlen(buf,count);
    }
    else if ( ref!=STDOUT)
    {
        return -1;
    }

    int i;
    for (i = 0; buf[i] && i < count; i++)
    {
        switch (buf[i])
        {
        case '\n':
            ncNewline();
            break;
        case '\b':
            ncBackspace();
            break;
        default:
            ncPrintChar(buf[i]);
        }
    }
    return i > 0 ? i : -1;
}

static int readFromStdin(char* buf,uint64_t count){
    char* actualBuffer;
    int sizeRead;
    int i=0;
    _sti();
    do{
        
        //waitForKeyboard();  No se como manejar el foreground todavia
        actualBuffer = kb_read();
        _hlt();
        
	}while ((sizeRead = sizeBuffer()) < count && actualBuffer[sizeRead - 1] != '\n');
    _cli();
    for (; i < count && i < sizeRead-1; i++) {
            buf[i] = actualBuffer[i];
    }
    resetBuffer();
    buf[i] = 0;
    return i;
}

static int read(unsigned int fd, char *buf, int count)
{
    uint64_t ref;
    if (buf == NULL)
        return -1;
    if((ref=getFdRef(fd))>=100 && ref<122 && ref%2==0){
        readFromPipe(ref,getProcessRunning(),count,buf);
        return count;//Por ahora, esto lo tengo que cambiar
    }
    else if ( ref!=STDIN)
    {
        return -1;
    }
    return readFromStdin(buf, count);
}

static int getCharSys(unsigned int ascii)
{
    int a=1;
    /*for (int i = 0; i < 1; i++)
    {
        a = kb_read();
        if (ascii && !PRINTABLE(a))
            i--;
    }*/
    return a;
}


static void getTime(char *buf)
{
    buf[0] = buf[3] = buf[6] = '0';
    buf[2] = buf[5] = ':';
    int h = hours();
    if (h < 10)
        numToStr(h, &buf[1]);
    else
        numToStr(h, &buf[0]);

    int m = minutes();

    if (m < 10)
        numToStr(m, &buf[4]);
    else
        numToStr(m, &buf[3]);

    int s = seconds();

    if (s < 10)
        numToStr(s, &buf[7]);
    else
        numToStr(s, &buf[6]);
    buf[8] = 0;
}



static void *malloc(size_t size)
{
    return mallocFun(size);
}

static void free(void *ptr)
{
    freeFun(ptr);
}

static void memState()
{
    char buf[MAX_STR_LENGTH];
    consult(buf);
    write(STDOUT, buf, MAX_STR_LENGTH, WHITE);
}

static uint64_t newProcess(void *(*funcion)(void *), void *argv, int argc[2],char* name)
{
    return createProcess(funcion,argv,argc[0],name,argc[1]);
}

static void endProcess(uint64_t pid)
{
    removeProcess(pid);
}

static void kill(uint64_t pid)
{
    removeProcess(pid);
}

static void getAllProcesses()
{
    char buf[MAX_STR_LENGTH_EXTENDED];
    listAllProcess(buf);
    write(STDOUT, buf, MAX_STR_LENGTH, WHITE);
}

static int nice(uint64_t pid,uint64_t priority)
{
    if (priority>MAX_PRIO)
        return -1;
    changePriority(pid,priority);
    return 1;
}
static void changeState(uint64_t pid, int status)
{
    if(status==READY)
        unblock(pid);
    else if(status==BLOCKED)
        block(pid);
    else 
        removeProcess(pid);

}

static void changeProcesses()
{
    
}

static int createSemaphore(char* name,uint64_t value)
{
    return createSem(name,value);
}

static int openSemaphore(char* name,uint64_t value)
{
    return semOpen(name,value);
}
static int closeSemaphore(char* semName)
{
    return semClose(semName);
}

static void getSemaphores()
{
    char buf[MAX_STR_LENGTH];
    sem(buf);
    write(STDOUT, buf, MAX_STR_LENGTH, WHITE);
}
static int waitSem(char* semaphore)
{
    return semWait(semaphore);
}

static int postSem(char* semaphore)
{
    return semPost(semaphore);
}

static int createPipe(int pipeFd[2])
{
    uint64_t pid = getProcessRunning();
    return pipeFun(pipeFd,pid);
}
//Esta creo q no va
static void openPipe(void *ptr)
{
}


static void getPipes()
{
    char buf[MAX_STR_LENGTH];
    listPipes(buf);
    write(STDOUT, buf, MAX_STR_LENGTH, WHITE);
}

static int getPidSys(){
    return (int)getProcessRunning();
}

static int dup2(uint64_t oldFd, uint64_t newFd){
    return dup(oldFd,newFd);
}

static void myYield(){
    //Lo pongo en 18 para que pase al next 
    forceTickCount();
    forceTimer();
}

static void exit(){
    killProcess(getProcessRunning());
}

static void wait(uint64_t pid){
    waitProcess(pid);
}

static void* getSharedMemoryBlock(uint64_t id){
    return getSharedMemory(id);
}