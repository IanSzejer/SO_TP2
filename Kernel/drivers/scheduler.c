#include "../include/lib.h"
#include "../include/scheduler.h"
#include "interrupts.h"
#include "videoD.h"
#include "../include/time.h"

#define MAX_SIZE 200
#define FIRST_PID 1
#define PROCESS_MEM_SIZE 10000 // El stack del proceso sera de 10000 bits
#define BASE_PRIORITY 1

int tickCountScheduler;
static int ready;
static ProcessList* firstList=NULL;
static ProcessNode* currentProcess=NULL;
static ProcessNode* foregroundProcess=NULL;
static ProcessNode* dummyProcess=NULL;
static uint64_t pidCounter = 1;
static int strlength(char* text);
static uint64_t getNewPid();
static ProcessList* recursiveAddList(ProcessNode* nodeToAdd, ProcessList* list) ;
static ProcessList* createList(ProcessNode* nodeToAdd, uint64_t priority);
static void checkReady(ProcessNode* node, ProcessList* list) ;
static ProcessList* removeRecursiveList(ProcessList* list, ProcessNode* process);
static ProcessNode* removeRecursiveNode(ProcessNode* node, ProcessNode* node2,ProcessNode* prevNode, ProcessList* list);
static void* getNextReady();
static ProcessList* findList(uint64_t pid,ProcessList* list);
static ProcessNode* getRecursiveNextReady(ProcessNode* current,ProcessList* list) ;
static void initiateFd(ProcessNode* newProcess);
static uint64_t changeState(uint64_t pid, states newState);
static ProcessList* change(ProcessList* list, ProcessNode* process);
static ProcessNode* changeNode(ProcessNode* node, ProcessNode* node2, ProcessList* list);
static int printInitial(char* buf);
static int printProcess(ProcessNode* node,char* buf);
static ProcessNode* findNode(uint64_t pid);
static char processInfo[]="PID      NAME        PRIORIDAD       STACK       BASE_POINTER    FOREGROUND";

// Cuando cree un proceso nuevo voy a querer q tenga otro pid
static uint64_t getNewPid() {
    return pidCounter++;
}

uint64_t getPid(){
    return currentProcess->pcb.pid;
}

uint64_t getProcessRunning(){
    return currentProcess->pcb.pid;
}

void dummy(){
    _hlt();
}

void initializeScheduler(void* (*funcion)(void*)) {
    _cli();
    createProcess(funcion,NULL,0,"shell",FOREGROUND);
    _sti();
}

void forceTickCount(){
    tickCountScheduler=19;
}
// Como argumento recibe un puntero a funcion, como es un proceso no se que parametros recibe
// Por ahora digo que devuelve void*, por decreto recibe hasta 3 argumentos de tama??o uint64_t
uint64_t createProcess(void* (*funcion)(void*), char* argv, int argc,char* processName,int processContext) {
    int c=0,j=0;
    char** arguments = (char**)mallocFun(argc*sizeof(char*));
    while(c < argc){
        int k=0;
        arguments[c]=(char*)mallocFun(21*sizeof(char));
        while(argv[j]){
            
            *(arguments[c]+k)=argv[j];
            k++;
            j++;
        }
        *(arguments+k) = 0;
        j++;
        c++;
    }
    void* stack = mallocFun(PROCESS_MEM_SIZE);
    
    void* stackTopPtr =(void*) createContext((uint64_t)stack + PROCESS_MEM_SIZE, funcion, argc,(char (*)[21]) arguments);
    ProcessNode* newProcess =(ProcessNode*) mallocFun(sizeof(ProcessNode));
    newProcess->pcb.rbp=stack+PROCESS_MEM_SIZE-1;
    memcpy(newProcess->pcb.name,processName,strlength(processName)*sizeof(char));
    newProcess->priority = BASE_PRIORITY;
    newProcess->pcb.pid = getNewPid();
    newProcess->pcb.rsp = stackTopPtr; 
    newProcess->pcb.processStartingMem= stack;
    newProcess->pcb.state = READY;
    newProcess->pcb.context=BACKGROUND; //Arranca en back siempre
    newProcess->pcb.fd[0].fd = 0;
    newProcess->pcb.fd[0].reference = STDINT;
    newProcess->pcb.fd[1].fd = 1;
    newProcess->pcb.fd[1].reference = STDOUT;
    newProcess->pcb.fd[2].fd = 2;
    newProcess->pcb.fd[2].reference = STDERR;
    newProcess->pcb.waitingProcess=0;
    newProcess->next=NULL;
    newProcess->pcb.context=processContext;
    newProcess->pcb.prevForegroundPid=0;
    if (currentProcess==NULL)
        newProcess->pcb.ppid=0;//Caso solo de la shell
    else{
        newProcess->pcb.ppid=currentProcess->pcb.pid;
    }
    for (int i = 0; i < argc; i++)  //Copio los argumentos
      memcpy(newProcess->pcb.argv[i], arguments[i],strlength(arguments[i])*sizeof(char));
    initiateFd(newProcess);
    // faltan cosas del pcb pero queria verlo con ustedes
    addProcess(newProcess);
    // asignar prioridad
    if(processContext==FOREGROUND){
        if(foregroundProcess!=NULL){
            newProcess->pcb.prevForegroundPid=foregroundProcess->pcb.pid;
            block(newProcess->pcb.prevForegroundPid);     //Bloqueo el padre de front hasta q termine
        }
        foregroundProcess=newProcess;
    }
    return newProcess->pcb.pid;
}
/*  
--->
c
    n
 --> a
*/


static int strlength(char* text){
    int i=0;
    while(text[i]!='\0'){
        i++;
    }
    return i+1;
}

static ProcessList* createList(ProcessNode* nodeToAdd, uint64_t priority) {
    ProcessList* newList = ( ProcessList*)mallocFun(sizeof(ProcessList));
    newList->nextList = NULL;
    newList->first = nodeToAdd;
    newList->last = nodeToAdd;
    newList->priority = priority;
    newList->ready = 0;
    newList->size = 0;
    return newList;
}

static void checkReady(ProcessNode* node, ProcessList* list) {
    if (node->pcb.state == READY){
        list->ready++;
        ready++;
    }
    list->size++;
}

static ProcessList* recursiveAddList(ProcessNode* nodeToAdd, ProcessList* list) {
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
        newList->nextList = list;
        checkReady(nodeToAdd, newList);
        return newList;
    }
    return list;
}

void addProcess(ProcessNode* nodeToAdd) {
    if (nodeToAdd == NULL) {
        return;
    }
    firstList = recursiveAddList(nodeToAdd, firstList);
}

void removeProcess(uint64_t pid) {
    ProcessNode* process=findNode(pid);
    if (process!=NULL)
        firstList = removeRecursiveList(firstList, process);
}

static ProcessList* removeRecursiveList(ProcessList* list, ProcessNode* process) {
    if (list == NULL)
        return NULL;
    
    // si encontre la lista
    if (list->priority == process->priority) {
        //busco el nodo y lo elimino
        list->first = removeRecursiveNode(list->first, process,NULL, list);
    } else if (list->priority > process->priority) {
        //si me pase fin
        return list;
    }

    list->nextList = removeRecursiveList(list->nextList, process);
    return list;
}


static ProcessNode* removeRecursiveNode(ProcessNode* node, ProcessNode* node2,ProcessNode* prevNode, ProcessList* list) {
    ProcessNode* aux;
    if (node == NULL) {
        return NULL;
    }
    if (node->pcb.pid == node2->pcb.pid) {
        if(node->pcb.state == READY){
            list->ready--;
            ready--;
        }
        aux = node->next;
        if(list->last==node){        //Tengo que modificar el last
            list->last=prevNode;
        }
        if(node->pcb.context==FOREGROUND){//Si era de front, libero el proceso que lo creo
            unblock(node->pcb.prevForegroundPid);
            foregroundProcess=findNode(node->pcb.prevForegroundPid);
        }
        freeFun(node->pcb.processStartingMem);     //Libero el stack del proceso
        freeFun(node);              //Libero el espacio reservado para el nodo
        list->size--;
        return aux;
    } 
    node->next = removeRecursiveNode(node->next, node2,node, list);
    return node;
}

static void* getNextReady() {
    // Si el proceso actual termino lo elimino
    ProcessList* currentList=findList(currentProcess->pcb.pid,firstList);
    ProcessNode* aux=currentProcess->next;
    if (currentProcess->pcb.state == KILLED) {
        removeProcess(currentProcess->pcb.pid);
    }
    
    if(ready==0){
        
        currentProcess =dummyProcess;
        return (dummyProcess->pcb.rsp);
    }
    currentProcess = getRecursiveNextReady(aux,currentList);
    tickCountScheduler=0;
    return currentProcess->pcb.rsp;
    
}

static ProcessList* findList(uint64_t pid,ProcessList* list){
    ProcessNode* aux=list->first;
    while(aux!=NULL){
        if(aux->pcb.pid==pid){
            return list;
        }
        aux=aux->next;
    }
    return findList(pid,list->nextList);
}

static ProcessNode* getRecursiveNextReady(ProcessNode* current,ProcessList* list) {
    if(list==NULL){
        list=firstList;
        current=list->first;
        return getRecursiveNextReady(current, list);
    }
    if (current == NULL) {
        list=list->nextList;
        if(list!=NULL)
        current=list->first;
        return getRecursiveNextReady(current,list);
    }

    if (current->pcb.state == BLOCKED) {
        return getRecursiveNextReady(current->next,list);
    }
    return current;
}

static void initiateFd(ProcessNode* newProcess) {
    for (int i = 3; i < FD_AMOUNT_PER_PROCESS; i++) {
        newProcess->pcb.fd[i].fd = 11;
        newProcess->pcb.fd[i].reference = 11;
    }
}

uint64_t killProcess(uint64_t pid)
{  
    ProcessNode* process= findNode(pid);
    uint16_t done = changeState(pid, KILLED);
    if(process->pcb.waitingProcess>0){
        unblock(process->pcb.waitingProcess);
        process->pcb.waitingProcess=0;
    }
    if (pid == currentProcess->pcb.pid)
        forceTimer();
    return done;
}


uint64_t unblock(uint64_t pid) {
    if (pid < FIRST_PID)
        return -1;
    return changeState(pid, READY);
}

uint64_t block(uint64_t pid) {
    if (pid < FIRST_PID)
        return -1;
    uint64_t toReturn = changeState(pid, BLOCKED);
    if (pid == currentProcess->pcb.pid)
    {
        forceTimer();
    }
    return toReturn;
}

static uint64_t changeState(uint64_t pid, states newState) {
    struct processNode* processNode;

    processNode = findNode(pid);
    ProcessList* list = findList(pid,firstList);
    if(newState == KILLED){
        if (processNode->pcb.state == READY) {
            list->ready--;
            ready--;
        }
        processNode->pcb.state=KILLED;
        return 0;
    }

    if (processNode == NULL) {
        return -1;
    }

    if (processNode->pcb.state == newState) {
        return 1;
    }

    if (processNode->pcb.state != READY && newState == READY) {
        list->ready++;
        ready++;
    } else if (processNode->pcb.state == READY && newState != READY) {
        list->ready--;
        ready--;
    }

    processNode->pcb.state = newState;
    return 0;
}

void* tickInterrupt(void* rsp) {
    timer_handler();
    
    
    if (currentProcess!=NULL){
        currentProcess->pcb.rsp=rsp;        //Guardo el rsp para el contexto
        if(currentProcess->pcb.state==BLOCKED || currentProcess->pcb.state==KILLED){
            getNextReady();
            return currentProcess->pcb.rsp;
        }
        tickCountScheduler++;
        if (tickCountScheduler > 18 - 2*currentProcess->priority) {
            getNextReady();
        }
        return currentProcess->pcb.rsp;     
    }
    

    if (currentProcess == NULL) {
        if(firstList==NULL){
           return rsp;         //Si no hay ninguno prooceso retorno el rsp que vino
        }
        currentProcess=firstList->first;
    }
    return currentProcess->pcb.rsp;        
}

void* dummyinterrupt(void* rsp){
    timer_handler();
    return rsp;
}

void changePriority(uint64_t pid, uint64_t newPriority) {
    ProcessNode* current=findNode(pid);

    if(current == NULL || current->priority == newPriority){
        return;
    }

    firstList = change(firstList, current);
    //como desaparecio de la lista pero no fue eliminado puedo acceder a current
    //le cambio la prioridad y lo agrego a la lista
    current->priority = newPriority;
    current->next=NULL;
    
    addProcess(current);
    
}

static ProcessList* change(ProcessList* list, ProcessNode* process) {
    if (list == NULL)
        return NULL;
    // si encontre la lista
    if (list->priority == process->priority) {
        list->first = changeNode(list->first, process, list);
        return list;
    } else if (list->priority > process->priority) {
        return list;
    }

    list->nextList = change(list->nextList, process);
    return list;
}


static ProcessNode* changeNode(ProcessNode* node, ProcessNode* node2, ProcessList* list) {
    
    if (node == NULL) {
        return NULL;
    }

    if (node->pcb.pid == node2->pcb.pid) {
        if(node->pcb.state==READY){
            ready--;
            list->ready--;
        }
        list->size--;
        //si lo encontre "lo salteo" -> hago q apunten al next y desaparezco de la lista
        return node->next;
    } else if (node->priority > node2->priority) {
        return node;
    }

    node->next = changeNode(node->next, node2, list);
    return node;
}


void listAllProcess(char* buf) {
    buf += printInitial(buf);
    *(buf++)='\n';
    ProcessList* aux = firstList;
    while (aux != NULL) {
        ProcessNode* nodeAux = aux->first; 
        while (nodeAux != NULL){
            buf += printProcess(nodeAux,buf);
            *(buf++)='\n';
            nodeAux = nodeAux->next;
        } 
        aux = aux->nextList;
    }
    *(buf++)='\0';
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
    int c=strlength(node->pcb.name)-1;
    memcpy(buf,node->pcb.name,c);     //Agrego todo el nombre menos el \0
    i+=c;
    buf+=c;
    *(buf++)='\t';
    *(buf++)='\t';
    i+=2;
    c=0;
    c = numToStr(node->priority,buf);
    i+=c;
    buf+=c;
    *(buf++)='\t';
    *(buf++)='\t';
    i+=2;
    *(buf++)='0';
    *(buf++)='x';
    i+=2;
    c=0;
    c= hexaToString((uint64_t)node->pcb.rsp,buf);
    i+=c;
    buf+=c;
    *(buf++)='\t';
    *(buf++)='\t';
    i+=2;
    *(buf++)='0';
    *(buf++)='x';
    i+=2;
    c=0;
    c= hexaToString((uint64_t)node->pcb.rbp,buf);
    i+=c;
    buf+=c;
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
    return i;
}

static ProcessNode* findNode(uint64_t pid) {
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


uint64_t getFdRef(uint64_t fd){
    return currentProcess->pcb.fd[fd].reference;
}

//Copio la referencia de fdOld a fdNew si es que existen, sino retorno -1
int dup(uint64_t fdOld,uint64_t fdNew){
    if(currentProcess->pcb.fd[fdOld].fd==11 || currentProcess->pcb.fd[fdNew].fd==11 )
        return -1;
    currentProcess->pcb.fd[fdNew].reference=currentProcess->pcb.fd[fdOld].reference;
    return 0;
}

void waitProcess(uint64_t pid){
    ProcessNode* node= findNode(pid);
    node->pcb.waitingProcess=currentProcess->pcb.pid;
    block(currentProcess->pcb.pid);
}