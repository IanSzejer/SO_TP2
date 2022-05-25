#include "stdint.h"
#include "../include/lib.h"
#include "../include/context.h"
#include "../include/memoryDriverPropio.h"

#define MAX_SIZE 200
#define MAX_NAME 100
#define FIRST_PID 1
#define PROCESS_SIZE 10000 // El stack del proceso sera de 10000 bits
#define BASE_PRIORITY 1
int tickCount typedef struct
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

typedef struct pcb_t
{
    char name[MAX_NAME];
    uint64_t pid;
    uint64_t ppid; // parent pid
    uint64_t rsp;
    uint64_t rbp;
    uint64_t priority;
    uint64_t tickets;
    states state;
    context context; // 1 -> FOREGROUND, 0 -> BACKGROUND
    uint64_t fdIn;
    uint64_t fdOut;
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
    ProcessList *nextList;
    uint64_t priority;
    uint32_t size;
    uint32_t nReady;
} ProcessList;

// lista 20{procesos:proceso1}
static ProcessList *firstList;
static ProcessNode *currentProcess;
static ProcessNode *dummyProcess;
static uint64_t pidCounter = 1;

void *createContext(void *stack, uint16_t *arguments, void *(*funcion)(void *), int argc);

// Cuando cree un proceso nuevo voy a querer q tenga otro pid
static uint64_t getNewPid()
{
    return pidCounter++;
}

void initializeScheduler(char *argv[])
{
    firstList = NULL;
    currentProcess = NULL;
    run(hlt);
}
// Como argumento recibe un puntero a funcion, como es un proceso no se que parametros recibe
// Por ahora digo que devuelve void*, por decreto recibe hasta 3 argumentos de tamaño uint64_t
void createProcess(void *(*funcion)(void *), void *argv, int argc)
{
    uint64_t *arguments = mallocFun(3 * sizeof(uint16_t));
    memcpy(arguments, argv, 3 * uint64_t);
    void *stack = mallocFun(PROCESS_SIZE);
    void *stackTopPtr = createContext(stack, arguments, funcion, argc);

    ProcessNode *newProcess = mallocFun(sizeof(ProcessNode));
    newProcess->priority = BASE_PRIORITY;
    newProcess->pcb.pid = getNewPid();
    newProcess->pcb.fdIn = 0;
    newProcess->pcb.fdOut = 1;
    newProcess->pcb.state = stackTopPtr; // Aca se tiene que empezar a popear, no estoy seguro
    // faltan cosas del pcb pero queria verlo con ustedes
    addProcess(newProcess);
    // asignar prioridad
}

ProcessList *createList(ProcessNode *nodeToAdd, uint64_t priority)
{
    ProcessList *newList = NULL;
    newList->nextList = NULL;
    newList->first = nodeToAdd;
    newList->last = nodeToAdd;
    newList->priority = priority;
    newList->nReady = 0;
    newList->size = 0;
}

void checkReady(ProcessNode *node, ProcessList *list)
{
    if (node->pcb.state == READY)
        list->nReady++;
    list->size++;
}

ProcessList *recursiveAddList(ProcessNode *nodeToAdd, ProcessList *list)
{
    if (list == NULL)
    {
        ProcessList *newList = createList(nodeToAdd, nodeToAdd->priority);
        checkReady(nodeToAdd, newList);
        return newList;
    }

    if (list->priority == nodeToAdd->priority)
    {
        // se convierte en el proximo del ultimo
        list->last->next = nodeToAdd;
        // y ahora es el ultimo
        list->last = nodeToAdd;
        checkReady(nodeToAdd, list);
    }
    else if (list->priority < nodeToAdd->priority)
    {
        list->nextList = recursiveAddList(nodeToAdd, list->nextList);
    }
    else
    {
        ProcessList *newList = createList(nodeToAdd, nodeToAdd->priority);
        newList->nextList = list->nextList;
        checkReady(nodeToAdd, newList);
        return newList;
    }
    return list;
}

ProcessNode *addProcess(ProcessNode *nodeToAdd)
{
    if (nodeToAdd == NULL)
    {
        return;
    }

    firstList = recursiveAddList(nodeToAdd, firstList);
}

ProcessNode *removeProcess(ProcessNode *process)
{
    firstList = removeRecursiveList(firstList, process);
}
ProcessList *removeRecursiveList(ProcessList *list, ProcessNode *process)
{
    ProcessList *aux;
    int deleted;
    int ready;
    if (list == NULL)
        return NULL;
    if (list->priority == process->priority)
    {
        if (list->size == 1)
        {
            aux = list->nextList;
            free(list);
            return aux;
        }
        if (process->pcb.state == READY)
            ready++;
        removeRecursiveProcess(list->first, process, &deleted);
        if (deleted)
        {
            list->size--;
            if (ready)
            {
                list->nReady--;
            }
        }
    }
    else if (list->priority > process->priority)
    {
        return list;
    }
    list->nextList = removeRecursiveList(list->nextList, process);
    return list;
}
ProcessNode *removeRecursiveNode(ProcessNode *node, ProcessNode *node2, int *deleted)
{
    ProcessNode *aux;
    if (node == NULL)
    {
        return NULL;
    }
    if (node->pcb.pid == node2->pcb.pid)
    {
        aux = node->next;
        free(node);
        *deleted++;
        return aux;
    }
    else if (node->pcb.pid > node2->pcb.pid)
    {
        return node;
    }
    node->next = removeRecursiveNode(node->next, node2, deleted);
}

static ProcessNode *getNextReady()
{
    // Si el proceso actual termino lo elimino
    if (currentProcess->pcb.state == KILLED)
    {
        ProcessNode *aux = removeProcess(currentProcess->pcb.pid);
    }

    // Si la prioridad del proximo es mayor a la del primero => current vuelve al inicio
    // Sigo con la prioridad baja hasta q termine
    currentProcess = getRecursiveNextReady(currentProcess->next);
    // if(currentProcess->next->priority > firstList->first->priority){
    //     currentProcess =  firstList->first;
    // }else{
    //     currentProcess = currentProcess->next;
    // }
    return currentProcess;
}

static ProcessNode *getRecursiveNextReady(ProcessNode *current)
{
    if (current == NULL)
    {
        return -1;
    }

    if (current->pcb.state == BLOCKED)
    {
        return getRecursiveNextReady(current->next);
    }

    if (current->priority <= firstList->first->priority)
    {
        return current;
    }
    else
    {
        return firstList->first;
    }
}

uint64_t unblock(uint64_t pid)
{
    if (pid < FIRST_PID)
        return -1;
    return changeState(pid, READY);
}

uint64_t block(uint64_t pid)
{
    if (pid < FIRST_PID)
        return -1;
    return changeState(pid, BLOCKED);
}

uint64_t changeState(uint64_t pid, states newState)
{
    struct processNode *processNode = getProcess(pid);
    if (processNode == NULL)
    {
        return -1;
    }

    if (processNode->pcb.state == newState)
    {
        return 1;
    }

    if (processNode->pcb.state != READY && newState == READY)
    {
        firstList->nReady++;
    }
    else if (processNode->pcb.state == READY && newState != READY)
    {
        firstList->nReady--;
    }

    processNode->pcb.state = newState;
    return 0;
}

void *createContext(void *stack, uint16_t *arguments, void *(*funcion)(void *), int argc)
{
    StackFrame_t *stackStruct = (StackFrame_t *)stack;
    stackStruct->base = stack;
    stackStruct->ss = 0x0;
    stackStruct->rsp = stackStruct->base;
    stackStruct->rflags = 0x202;
    stackStruct->cs = 0x8;
    stackStruct->rip = funcion;
    stackStruct->r15 = 0;
    stackStruct->r14 = 0;
    stackStruct->r13 = 0;
    stackStruct->r12 = 0;
    stackStruct->r11 = 0;
    stackStruct->r10 = 0;
    stackStruct->r9 = 0;
    stackStruct->r8 = 0;
    stackStruct->rsi = arguments;
    stackStruct->rdi = argc;
    stackStruct->rbp = stackStruct->base;
    stackStruct->rdx = 0;
    stackStruct->rcx = 0;
    stackStruct->rbx = 0;
    stackStruct->rax = 0;
    return &stackStruct->rax;
}

void tickInterrupt()
{
    if (currentProcess != NULL)
    {
        tickCount++;
        if (tickCount > 9 - currentProcess->priority)
        {
            getNextReady()
        }
    }

    if (currentProcess == NULL)
    {
        run(hlt);
    }
    else
        run(currentProcess)
}

void changePriority(ProcessNode *current, uint64_t newPriority){
    firstList = change(firstList, current)
                    current->priority = newPriority
        addProcess(current)

}

ProcessList *change(ProcessList *list, ProcessNode *process)
{
    ProcessList *aux;
    int deleted;
    int ready;
    if (list == NULL)
        return NULL;
    if (list->priority == process->priority)
    {
        if (list->size == 1)
        {
            aux = list->nextList;
            return aux;
        }
        if (process->pcb.state == READY)
            ready++;
        changeProcess(list->first, process, &deleted);
        if (deleted)
        {
            list->size--;
            if (ready)
            {
                list->nReady--;
            }
        }
    }
    else if (list->priority > process->priority)
    {
        return list;
    }
    list->nextList = change(list->nextList, process);
    return list;
}

ProcessNode *changeProcess(ProcessNode *node, ProcessNode *node2, int *deleted)
{
    ProcessNode *aux;
    if (node == NULL)
    {
        return NULL;
    }
    if (node->pcb.pid == node2->pcb.pid)
    {
        aux = node->next;
        *deleted++;
        return aux;
    }
    else if (node->pcb.pid > node2->pcb.pid)
    {
        return node;
    }
    node->next = changeProcess(node->next, node2, deleted);
}

ProcessNode *listAllProcess()
{
    ProcessNode toReturn[MAX_SIZE] = NULL;
    int i = 0;
    ProcessList *aux = firstList while (aux != NULL)
    {
        ProcessNode *nodeAux = aux->first while (nodeAux != NULL){
            toReturn[i++] = nodeAux
                // Luego voy a tener q printear
                // nombre, ID, prioridad, stack y base pointer, foreground y
                // cualquier otra variable que consideren necesaria.
                nodeAux = nodeAux->next} aux = aux->nextList
    }

    return toReturn;
}