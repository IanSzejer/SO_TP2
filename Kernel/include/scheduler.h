#ifndef SCHEDULER_H
#define SCHEDULER_H
#define FD_AMOUNT_PER_PROCESS 10
#define STDINT 0
#define STDOUT 1
#define STDERR 2
#define MAX_NAME_SCHED 100
#define MAX_PRIO 10
#define FOREGROUND 1
#define BACKGROUND 0
#include "memoryDriver.h"


typedef enum
{
    READY,
    BLOCKED,
    KILLED
} states;

typedef struct fileDescriptors
{
    uint64_t fd;
    uint64_t reference;
}FileDescriptorsTable;

typedef struct pcb_t
{
    char name[MAX_NAME_SCHED];
    uint64_t pid;
    uint64_t ppid; // parent pid
    void* rsp;
    void* rbp;
    void* processStartingMem;
    uint64_t tickets;
    char argv[6][21];
    states state;
    int context; // 1 -> FOREGROUND, 0 -> BACKGROUND
    FileDescriptorsTable fd[FD_AMOUNT_PER_PROCESS];            //Maximo 10 fd 
    uint64_t waitingProcess;
    uint64_t prevForegroundPid;
} pcb_t;


typedef struct processNode
{
    pcb_t pcb;
    uint64_t priority;
    struct processNode *next;
} ProcessNode;

typedef struct processList
{
    ProcessNode *first;
    ProcessNode *last;
    struct processList *nextList;
    uint64_t priority;
    uint32_t size;
    uint32_t ready;
    ProcessNode* fg;
} ProcessList;

extern void forceTimer();

void* tickInterrupt();
uint64_t getProcessRunning();
void addPipe(uint64_t fd[2],uint64_t pid,uint64_t pipeRef,uint64_t pipeWriteRef);
void initializeScheduler(void* (*funcion)(void*));
uint64_t createProcess(void* (*funcion)(void*), char* argv, int argc,char* processName,int processContext);
void addProcess(ProcessNode *nodeToAdd);
void removeProcess(uint64_t pid);
uint64_t killProcess(uint64_t pid);
uint64_t unblock(uint64_t pid);
uint64_t block(uint64_t pid);
void changePriority(uint64_t pid, uint64_t newPriority);
void listAllProcess(char* buf);
uint64_t getPid();
uint64_t getFdRef(uint64_t fd);
int dup(uint64_t fdOld,uint64_t fdNew);
void* dummyinterrupt(void* rsp);        //Dummy para probar
void forceTickCount();
void waitProcess(uint64_t pid);

#endif