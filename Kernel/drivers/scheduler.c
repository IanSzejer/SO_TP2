#include "stdint.h"
#include "../include/lib.h"
#include "../include/memoryDriverPropio.h"
#include "../include/scheduler.h"

#define MAX_SIZE 200
#define FIRST_PID 1
#define PROCESS_SIZE 10000 // El stack del proceso sera de 10000 bits
#define BASE_PRIORITY 1

// lista 20{procesos:proceso1}
static ProcessList* firstList;
static ProcessNode* currentProcess;
static ProcessNode* dummyProcess;
static uint64_t pidCounter = 1;

// Cuando cree un proceso nuevo voy a querer q tenga otro pid
static uint64_t getNewPid() {
    return pidCounter++;
}

uint64_t getPid(){
    return currentProcess->pcb.pid;
}
uint64_t initializeScheduler(char* argv[]) {
    firstList = NULL;
    currentProcess = NULL;
    //crear dummyProcess
    return dummyProcess->pcb.rsp;

}
// Como argumento recibe un puntero a funcion, como es un proceso no se que parametros recibe
// Por ahora digo que devuelve void*, por decreto recibe hasta 3 argumentos de tamaño uint64_t
void createProcess(void* (*funcion)(void*), void* argv, int argc) {
    uint64_t* arguments = mallocFun(3 * sizeof(uint16_t));
    memcpy(arguments, argv, 3 * sizeof(uint64_t));
    void* stack = mallocFun(PROCESS_SIZE);
    void* stackTopPtr = createContext(stack, arguments, funcion, argc);

    ProcessNode* newProcess = mallocFun(sizeof(ProcessNode));
    newProcess->pcb.rbp=(uint64_t)stack;
    newProcess->priority = BASE_PRIORITY;
    newProcess->pcb.pid = getNewPid();
    newProcess->pcb.rsp = stackTopPtr; // Aca se tiene que empezar a popear, no estoy seguro
    newProcess->pcb.state = READY;
    newProcess->pcb.context=BACKGROUND; //Arranca en back siempre
    newProcess->pcb.fd[0].fd = 0;
    newProcess->pcb.fd[0].reference = STDINT;
    newProcess->pcb.fd[1].fd = 1;
    newProcess->pcb.fd[1].reference = STDOUT;
    newProcess->pcb.fd[2].fd = 2;
    newProcess->pcb.fd[2].reference = STDERR;
    newProcess->pcb.argv=arguments;
    initiateFd(newProcess);
    // faltan cosas del pcb pero queria verlo con ustedes
    addProcess(newProcess);
    // asignar prioridad
}

ProcessList* createList(ProcessNode* nodeToAdd, uint64_t priority) {
    ProcessList* newList = NULL;
    newList->nextList = NULL;
    newList->first = nodeToAdd;
    newList->last = nodeToAdd;
    newList->priority = priority;
    newList->nReady = 0;
    newList->size = 0;
}

void checkReady(ProcessNode* node, ProcessList* list) {
    if (node->pcb.state == READY)
        list->nReady++;
    list->size++;
}

ProcessList* recursiveAddList(ProcessNode* nodeToAdd, ProcessList* list) {
    if (list == NULL) {
        ProcessList* newList = createList(nodeToAdd, nodeToAdd->priority);
        checkReady(nodeToAdd, newList);
        return newList;
    }

    if (list->priority == nodeToAdd->priority) {
        // se convierte en el proximo del ultimo
        list->last->next = nodeToAdd;
        // y ahora es el ultimo
        list->last = nodeToAdd;
        checkReady(nodeToAdd, list);
    } else if (list->priority < nodeToAdd->priority) {
        list->nextList = recursiveAddList(nodeToAdd, list->nextList);
    } else {
        ProcessList* newList = createList(nodeToAdd, nodeToAdd->priority);
        newList->nextList = list->nextList;
        checkReady(nodeToAdd, newList);
        return newList;
    }
    return list;
}

ProcessNode* addProcess(ProcessNode* nodeToAdd) {
    if (nodeToAdd == NULL) {
        return;
    }

    firstList = recursiveAddList(nodeToAdd, firstList);
}

ProcessNode* removeProcess(uint64_t pid) {
    ProcessNode* process=getProcess(pid);
    firstList = removeRecursiveList(firstList, process);
}
ProcessList* removeRecursiveList(ProcessList* list, ProcessNode* process) {
    ProcessList* aux;
    int deleted;
    int ready;
    if (list == NULL)
        return NULL;
    if (list->priority == process->priority) {
        if (list->size == 1) {
            aux = list->nextList;
            freeFun(list);
            return aux;
        }
        if (process->pcb.state == READY)
            ready++;
        removeRecursiveProcess(list->first, process, &deleted);
        if (deleted) {
            list->size--;
            if (ready) {
                list->nReady--;
            }
        }
    } else if (list->priority > process->priority) {
        return list;
    }
    list->nextList = removeRecursiveList(list->nextList, process);
    return list;
}
ProcessNode* removeRecursiveNode(ProcessNode* node, ProcessNode* node2, int* deleted) {
    ProcessNode* aux;
    if (node == NULL) {
        return NULL;
    }
    if (node->pcb.pid == node2->pcb.pid) {
        aux = node->next;
        freeFun(node->pcb.argv);    //Libero el espacio reservado para arg
        freeFun(node->pcb.rbp);     //Libero el stack del proceso
        freeFun(node);              //Libero el espacio reservado para el nodo
        *deleted++;
        return aux;
    } else if (node->pcb.pid > node2->pcb.pid) {
        return node;
    }
    node->next = removeRecursiveNode(node->next, node2, deleted);
}

static ProcessNode* getNextReady() {
    // Si el proceso actual termino lo elimino
    if (currentProcess->pcb.state == KILLED) {
        ProcessNode* aux = removeProcess(currentProcess->pcb.pid);
    }

    // Si la prioridad del proximo es mayor a la del primero => current vuelve al inicio
    // Sigo con la prioridad baja hasta q termine
    currentProcess = getRecursiveNextReady(currentProcess->next);
    // if(currentProcess->next->priority > firstList->first->priority){
    //     currentProcess =  firstList->first;
    // }else{
    //     currentProcess = currentProcess->next;
    // }
    if (currentProcess==NULL)
    return dummyProcess->pcb.rsp;
    return currentProcess->pcb.rsp;
}

static ProcessNode* getRecursiveNextReady(ProcessNode* current) {
    if (current == NULL) {
        return -1;
    }

    if (current->pcb.state == BLOCKED) {
        return getRecursiveNextReady(current->next);
    }

    if (current->priority <= firstList->first->priority) {
        return current;
    } else {
        return firstList->first;
    }
}

static void initiateFd(ProcessNode* newProcess) {
    for (int i = 3; i < FD_AMOUNT_PER_PROCESS; i++) {
        newProcess->pcb.fd[i].fd = 11;
        newProcess->pcb.fd[i].reference = 11;
    }
}

uint64_t unblock(uint64_t pid) {
    if (pid < FIRST_PID)
        return -1;
    return changeState(pid, READY);
}

uint64_t block(uint64_t pid) {
    if (pid < FIRST_PID)
        return -1;
    return changeState(pid, BLOCKED);
}

static uint64_t changeState(uint64_t pid, states newState) {
    struct processNode* processNode = getProcess(pid);
    if (processNode == NULL) {
        return -1;
    }

    if (processNode->pcb.state == newState) {
        return 1;
    }

    if (processNode->pcb.state != READY && newState == READY) {
        firstList->nReady++;
    } else if (processNode->pcb.state == READY && newState != READY) {
        firstList->nReady--;
    }

    processNode->pcb.state = newState;
    return 0;
}

void* createContext(void* stack, uint16_t* arguments, void* (*funcion)(void*), int argc) {
    StackFrame_t* stackStruct = (StackFrame_t*)stack;
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

uint64_t tickInterrupt() {
    if (currentProcess != NULL) {
        tickCount++;
        if (tickCount > 9 - currentProcess->priority) {
            getNextReady();
        }
    }

    if (currentProcess == NULL) {
        return dummyProcess->pcb.rsp;
    } else
        return currentProcess->pcb.rsp;
}

void changePriority(ProcessNode* current, uint64_t newPriority) {
    firstList = change(firstList, current);
    current->priority = newPriority;
    addProcess(current);
}

static ProcessList* change(ProcessList* list, ProcessNode* process) {
    ProcessList* aux;
    int deleted;
    int ready;
    if (list == NULL)
        return NULL;
    if (list->priority == process->priority) {
        if (list->size == 1) {
            aux = list->nextList;
            return aux;
        }
        if (process->pcb.state == READY)
            ready++;
        changeProcess(list->first, process, &deleted);
        if (deleted) {
            list->size--;
            if (ready) {
                list->nReady--;
            }
        }
    } else if (list->priority > process->priority) {
        return list;
    }
    list->nextList = change(list->nextList, process);
    return list;
}

static ProcessNode* changeProcess(ProcessNode* node, ProcessNode* node2, int* deleted) {
    ProcessNode* aux;
    if (node == NULL) {
        return NULL;
    }
    if (node->pcb.pid == node2->pcb.pid) {
        aux = node->next;
        *deleted++;
        return aux;
    } else if (node->pcb.pid > node2->pcb.pid) {
        return node;
    }
    node->next = changeProcess(node->next, node2, deleted);
}

ProcessNode* listAllProcess(char* buf) {
    buf += printInitial(buf);
    *(buf++)='\n';
    ProcessNode* toReturn[MAX_SIZE] = NULL;
    int i = 0;
    ProcessList* aux = firstList;
    while (aux != NULL) {
        ProcessNode* nodeAux = aux->first; 
        while (nodeAux != NULL){
            buf += printProcess(nodeAux,buf);
            *(buf++)='\n';
            toReturn[i++] = nodeAux;
            nodeAux = nodeAux->next;
        } 
        aux = aux->nextList;
    }

    return toReturn;
}

static int printInitial(char* buf){
    int i=0;
    while(processInfo[i]!='\0'){
        *(buf++)=processInfo[i];
        i++;
    }
    return i;
}

static int printProcess(ProcessNode* node,char* buf){
    int i=0;
    i += numToStr(node->pcb.pid,buf);
    buf+=i;
    *(buf++)='\t';
    *(buf++)='\t';
    i+=2;
    /*
    nombre
    */
    i += numToStr(node->priority,buf);
    buf+=i;
    *(buf++)='\t';
    *(buf++)='\t';
    i+=2;
    *(buf++)='0';
    *(buf++)='x';
    i+=2;
    i += hexaToString(node->pcb.rsp,buf);
    buf+=i;
    *(buf++)='\t';
    *(buf++)='\t';
    i+=2;
    *(buf++)='0';
    *(buf++)='x';
    i+=2;
    i += hexaToString(node->pcb.rbp,buf);
    buf+=i;
    *(buf++)='\t';
    *(buf++)='\t';
    i+=2;
    if(node->pcb.context==FOREGROUND){
        *(buf++)='T';
        *(buf++)='r';
        *(buf++)='u';
        *(buf++)='e';
        i+=4;
    }else{
        *(buf++)='F';
        *(buf++)='a';
        *(buf++)='l';
        *(buf++)='s';
        *(buf++)='e';
        i+=5;
    }
    *(buf++)='\0';
    i++;
    return i;
}

ProcessNode* findNode(uint64_t pid) {
    ProcessList* currentList = firstList;
    ProcessNode* currentNode;
    while (currentList != NULL) {
        currentNode = currentList->first;
        while (currentNode != NULL) {
            if (currentNode->pcb.pid == pid)
                return currentNode;
            currentNode = currentNode->next;
        }
        currentList = currentList->nextList;
    }
    return NULL;
}

void addPipe(uint64_t fd[2], uint64_t pid, uint64_t pipeReadRef, uint64_t pipeWriteRef) {
    ProcessNode* node = findNode(pid);
    if (node == NULL) {
        fd[0] = 0;
        fd[1] = 0;
        return;
    }
    int found = 0;
    for (int i = 0; i < FD_AMOUNT_PER_PROCESS && found < 2; i++) {
        if (node->pcb.fd[i].fd != i) { // Si en posicion 4 no esta el fd 4 es xq no existe
            node->pcb.fd[i].fd = i;
            node->pcb.fd[i].reference = pipeReadRef;
            fd[0] = i;
            found++;
        }
        if (node->pcb.fd[i].fd != i) { // Si en posicion 4 no esta el fd 4 es xq no existe
            node->pcb.fd[i].fd = i;
            node->pcb.fd[i].reference = pipeWriteRef;
            fd[1] = i;
            found++;
        }
    }
    // Si sale del for es que no hay 2 fd para asignar un pipe
    fd[0] = 0;
    fd[1] = 0;
}