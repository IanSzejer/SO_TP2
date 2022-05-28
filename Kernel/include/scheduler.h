#ifndef SCHEDULER_H
#define SCHEDULER_H
#define FD_AMOUNT_PER_PROCESS 10
#define STDINT 0
#define STDOUT 1
#define STDERR 2
#define MAX_NAME_SCHED 100
#define MAX_PRIO 10


typedef struct stackframe
{
    // Registers restore context
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx; //pid
    uint64_t rdx; 
    uint64_t rbp;
    uint64_t rdi; //argc
    uint64_t rsi; //argv
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;

    // iretq hook
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
    uint64_t base;
} StackFrame_t;

typedef enum
{
    FOREGROUND,
    BACKGROUND,

} context;

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
    uint64_t priority;
    uint64_t tickets;
    uint64_t* argv;
    states state;
    context context; // 1 -> FOREGROUND, 0 -> BACKGROUND
    FileDescriptorsTable fd[FD_AMOUNT_PER_PROCESS];            //Maximo 10 fd 
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
void initializeScheduler();
uint64_t createProcess(void* (*funcion)(void*), void* argv, int argc,char* processName);
void addProcess(ProcessNode *nodeToAdd);
void removeProcess(uint64_t pid);
uint64_t killProcess(uint64_t pid);
uint64_t unblock(uint64_t pid);
uint64_t block(uint64_t pid);
void* createContext(void *stack, uint64_t *arguments, void *(*funcion)(void *), int argc);
void changePriority(uint64_t pid, uint64_t newPriority);
ProcessNode *listAllProcess(char* buf);
uint64_t getPid();
uint64_t getFdRef(uint64_t fd);


#endif