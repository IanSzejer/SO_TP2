#include "stdint.h"
#include "../include/lib.h"
#define MAX_NAME 100
#define FIRST_PID 1

typedef enum {
    FOREGROUND,
    BACKGROUND,

} context;

typedef enum {
    READY,
    BLOCKED,
    KILLED
} states;

typedef struct pcb_t {
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

typedef struct processNode {
    pcb_t pcb;
    uint64_t priority;
    struct processNode* next;
} ProcessNode;

typedef struct processList {
    ProcessNode* first; // No se como definiriamos en ProccesNode
    ProcessNode* last;
    ProcessList* nextList;
    uint64_t priority;
    uint32_t size;
    uint32_t nReady;
} ProcessList;

// lista 20{procesos:proceso1}
static ProcessList* firstList;
static ProcessNode* currentProcess;
static ProcessNode* dummyProcess;
static uint64_t pidCounter = 1;

// Cuando cree un proceso nuevo voy a querer q tenga otro pid
static uint64_t getNewPid() {
    return pidCounter++;
}

void initializeScheduler(char* argv[]) {
    firstList = NULL;
    currentProcess = NULL;
    // Me quiero fijar si me pasaron dummy o mem manager
    // if(&argv[2] == 'dummy'){
    //     // corro el dummy
    // }else{
    //     //corro memory manager
    // }
}

void createProcess() {
    // espacio para contexto
    ProcessNode* newProcess = malloc(sizeof(ProcessNode));
    // posicion de la primera instruccion
    // poner en la lista de procesos
    addProcess(newProcess);
    // asignar prioridad
}

// struct processNode * recursiveAddProcess(struct processNode * nodeToAdd, struct processNode * prevNode)
// {
//     if(prevNode == NULL){
//         firstList->last=nodeToAdd;
//         return nodeToAdd;
//     }

//     //Lo agrego al final de su prioridad
//     if(prevNode->priority > nodeToAdd->priority){
//         nodeToAdd->next = prevNode;
//         return nodeToAdd;
//     }else{
//         return recursiveAddProcess(nodeToAdd, prevNode->next);
//     }
// };

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

    // list->first = recursiveAddProcess(nodeToAdd, list->first);
    // if (list->first == NULL)
    //     list->first = nodeToAdd;
    // else
    //     list->last->next = nodeToAdd;

    // list->last = nodeToAdd;
    // nodeToAdd->next = NULL;
    // if (nodeToAdd->pcb.state == READY)
    //     firstList->nReady++;
    // firstList->size++;
}

ProcessNode* removeProcess(ProcessNode* process) {
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
            free(list);
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
        free(node);
        *deleted++;
        return aux;
    } else if (node->pcb.pid > node2->pcb.pid) {
        return node;
    }
    node->next = removeRecursiveNode(node->next, node2, deleted);
}

ProcessNode* getProcess(uint64_t pid) {
    if (currentProcess->pcb.pid == pid)
        return currentProcess;

    struct processNode* aux = firstList->first;

    while (aux != NULL) {
        if (aux->pcb.pid == aux) {
            return aux;
        }
        aux = aux->next;
    }
    return NULL;
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
    return currentProcess;
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

uint64_t unblock(uint64_t pid) {
    if (pid < FIRST_PID)
        return -1;
    return changeState(pid, READY);
}

uint64_t changeState(uint64_t pid, states newState) {
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