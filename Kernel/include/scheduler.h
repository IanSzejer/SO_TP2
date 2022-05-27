#ifndef SCHEDULER_H
#define SCHEDULER_H
#define FD_AMOUNT_PER_PROCESS 10
#define STDINT 0
#define STDOUT 1
#define STDERR 2
#define MAX_NAME 100
#define MAX_PRIO 10
int tickCount;

typedef struct stackframe
{
    // Registers restore context
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
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
    char name[MAX_NAME];
    uint64_t pid;
    uint64_t ppid; // parent pid
    uint64_t rsp;
    uint64_t rbp;
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
    uint32_t nReady;
    ProcessNode* fg;
} ProcessList;

static char processInfo[]="PID      NAME        PRIORIDAD       STACK       BASE_POINTER    FOREGROUND";

void addPipe(uint64_t fd[2],uint64_t pid,uint64_t pipeRef,uint64_t pipeWriteRef);
uint64_t initializeScheduler(char *argv[]);
void createProcess(void *(*funcion)(void *), void *argv, int argc);
ProcessList *createList(ProcessNode *nodeToAdd, uint64_t priority);
void checkReady(ProcessNode *node, ProcessList *list);
ProcessNode *addProcess(ProcessNode *nodeToAdd);
ProcessNode *removeProcess(uint64_t pid);
uint64_t unblock(uint64_t pid);
uint64_t block(uint64_t pid);
void *createContext(void *stack, uint16_t *arguments, void *(*funcion)(void *), int argc);
uint64_t tickInterrupt();
void changePriority(ProcessNode *current, uint64_t newPriority);
ProcessNode *listAllProcess();
static void initiateFd(ProcessNode* newProcess);
void *createContext(void *stack, uint16_t *arguments, void *(*funcion)(void *), int argc);
static int printInitial(char* buf);
uint64_t getPid();
#endif