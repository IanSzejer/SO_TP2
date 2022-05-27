#include "pipeManager.h"
//#include "scheduler.h" INCLUIRLO CUANDO LO META CON EL RESTO DEL CODIGO


typedef unsigned long long uint64_t;
// Estructura que tendra un pipe


static Pipe* pipeArray[PIPE_AMOUNT] = {0};
static char pipePhrase1[] = "Pipe ID: ";
static char pipePhrase2[] = "esta esperando que lo lean";
static char pipePhrase3[] = "no esta esperando que lo lean";
// void startFdOccupied(PipeList* list);

// static PipeList* list=NULL;
static uint64_t pipeId = 1;
static uint64_t getPipeId() {
    return pipeId++;
}

/*Funcion que se encarga de crear el pipe y devolver los fd
**  Recibe: array de 2 int donde guardara los fd creados
**  Return: -1 por error y 0 si se pudieron crear correctamente
*/
int pipeFun(int pipeFd[2], int processId) {

    Pipe* newPipe = (Pipe*)mallocFun(sizeof(Pipe));
    if (newPipe == NULL)
        return -1;
    int found = 0;
    for (int i = 0; i < PIPE_AMOUNT && !found; i++) {
        if (pipeArray[i] != 0) {
            pipeArray[i] = newPipe;
            found = 1;
        }
    }
    if (!found)
        return -1;
    // Creo el pipe completo
    newPipe->bufferInit = (char*)mallocFun(BUFF_SIZE);
    if (newPipe->bufferInit == NULL)
        return -1;
    newPipe->bufferEnd=newPipe->bufferInit+BUFF_SIZE;
    newPipe->bufferWriten = 0;
    newPipe->readPointer = newPipe->bufferInit;
    newPipe->writePointer = newPipe->bufferInit;
    newPipe->firstReadWaitingList = NULL;
    newPipe->firstWriteWaitingList = NULL;
    newPipe->lastReadWaitingList = NULL;
    newPipe->lastWriteWaitingList = NULL;
    newPipe->writen = 0;
    newPipe->pipeId = getPipeId();
    newPipe->pipeReadRef = getReadRef(newPipe->pipeId);
    newPipe->pipeWriteRef = getWriteRef(newPipe->pipeId);
    newPipe->nextPipe = NULL;

    addPipe(pipeFd, processId, newPipe->pipeReadRef, newPipe->pipeWriteRef);
    return 0;
}

// Los ref arrancan en 100 y son 100 para read y 101 para write x ejemplo
static int getReadRef(uint64_t pipeId) {
    return pipeId * 2 + 100;
}
static int getWriteRef(uint64_t pipeId) {
    return pipeId * 2 + 101;
}

int writeInPipe(uint64_t writePipeId, uint64_t pid, int size, char* text) {
    int found = 0;
    int i = 0;
    for (; i < PIPE_AMOUNT && !found; i++) {
        if (pipeArray[i] != NULL && pipeArray[i]->pipeId == writePipeId)
            found = 1;
    }
    if (found == 0) {
        return -1;
    }
    if (pipeArray[i]->writen == 1) {
        addWritingUser(i, size, text,pid);
    } else {
        // Como el array es circular, copio hasta el final, incremento el text y copio el resto
        writeInBuffer(pipeArray[i],pid,size,text);
    }
}

int readFromPipe(uint64_t readPipeId, uint64_t pid, int size, char* text){
    int found = 0;
    int i = 0;
    for (; i < PIPE_AMOUNT && !found; i++) {
        if (pipeArray[i] != NULL && pipeArray[i]->pipeId == readPipeId)
            found = 1;
    }
    if (found == 0) {
        return -1;
    }
    if (pipeArray[i]->bufferWriten == 0) {          //No hay nada escrito
        addReadingUser(i, size, text,pid);
    } else {
        // Como el array es circular, copio hasta el final, incremento el text y copio el resto
        readFromBuffer(pipeArray[i],pid,size,text);
    }
}

//No estoy seguro si tiene q imprimir directo o devolver en un buffer
// Para el listado devuelvo un buffer con: PipeId,si tiene escrito y espera lectura
void listPipes(char* buf) {
    for (int i = 0; i < PIPE_AMOUNT; i++) {
        if (pipeArray[i] != 0)
            buf += printPipe(buf, pipeArray[i]);
    }
    *(buf++) = '\0';
}

static void readFromBuffer(Pipe* pipe,uint64_t pid,uint64_t size,char* text){
    memcpy(text,pipe->readPointer, min(pipe->bufferEnd - pipe->readPointer,min(size,pipe->bufferWriten)));
    uint64_t amountCopied =  min(pipe->bufferEnd - pipe->readPointer,min(size,pipe->bufferWriten));
    text += amountCopied/(sizeof(char));    //Lo aumento el tamaño copiado en chars
    size-=amountCopied;
    pipe->bufferWriten-=amountCopied;
    if(pipe->readPointer==pipe->bufferEnd ) //Debo volver al inicio del array si no esta lleno
        pipe->readPointer=pipe->bufferInit;
    else
        pipe->readPointer+=amountCopied;
    pipe->writen=0;
    if(pipe->bufferWriten == 0){
        if(size>0){
            addReadingUser(pipe, size, text,pid);         //Lo pongo en la cola de espera
            startWriting(pipe);
            return;
        }
    }
    if(size==0){
        startWriting(pipe);    
        return;
    }

    memcpy(text,pipe->readPointer, min(size,pipe->bufferWriten));
    amountCopied =  min(size,pipe->bufferWriten);
    size-=amountCopied;
    pipe->bufferWriten-=amountCopied; 
    pipe->readPointer += amountCopied;
    if(size==0){
        startWriting(pipe);    
        return;
    }
    text += amountCopied/(sizeof(char));    //Lo aumento el tamaño copiado en chars
    addreadingUser(pipe, size, text,pid);
    startWriting(pipe);
}

static void startWriting(Pipe* pipe){
    if(pipe->firstWriteWaitingList==NULL)
        return;     
    PipeUser* current=pipe->firstWriteWaitingList;
    PipeUser* aux;
    uint64_t pid;
    while(current!=NULL){
        pid=current->pid;
        aux=current->nextUser;
        freeFun(current);
        current=aux;
        unblock(pid);
    }
    pipe->firstWriteWaitingList=NULL;
    pipe->lastWriteWaitingList=NULL;
}


static void startreading(Pipe* pipe){
    if(pipe->firstReadWaitingList==NULL)
        return;     
    PipeUser* current=pipe->firstReadWaitingList;
    PipeUser* aux;
    uint64_t pid;
    while(current!=NULL){
        pid=current->pid;
        aux=current->nextUser;
        freeFun(current);
        current=aux;
        unblock(pid);
    }
    pipe->firstReadWaitingList=NULL;
    pipe->lastReadWaitingList=NULL;

}


static void writeInBuffer(Pipe* pipe,uint64_t pid,uint64_t size,char* text){
    
    memcpy(pipe->writePointer, text, min(pipe->bufferEnd - pipe->writePointer,min(size,BUFF_SIZE-pipe->bufferWriten)));
    uint64_t amountCopied =  min(pipe->bufferEnd - pipe->writePointer,min(size,BUFF_SIZE-pipe->bufferWriten));
    text += amountCopied/(sizeof(char));    //Lo aumento el tamaño copiado en chars
    size-=amountCopied;
    pipe->bufferWriten+=amountCopied;
    if(pipe->writePointer==pipe->bufferEnd ) //Debo volver al inicio del array si no esta lleno
        pipe->writePointer=pipe->bufferInit;
    else
        pipe->writePointer+=amountCopied;
    if(pipe->bufferWriten == BUFF_SIZE){
        pipe->writen=1;
        startreading(pipe);
        if(size>0){
            addWritingUser(pipe, size, text,pid);         //Lo pongo en la cola de espera
            return;
        }
    }
    if(size==0){
        startreading(pipe);
        return;
    }
    // Copio el resto del tamaño
    memcpy(pipe->writePointer, text, min(size,BUFF_SIZE-pipe->bufferWriten));      
    amountCopied =  min(size,BUFF_SIZE-pipe->bufferWriten);
    size-=amountCopied;
    pipe->bufferWriten+=amountCopied; 
    pipe->writePointer += amountCopied;
    if(size==0){
        startreading(pipe);
        return;
    }
    text += amountCopied/(sizeof(char));    //Lo aumento el tamaño copiado en chars
    
    pipe->writen = 1;
    addWritingUser(pipe, size, text,pid);
    startreading(pipe);
}

static uint64_t min(uint64_t num1,uint64_t num2){
    return num1<num2? num1 : num2;
}

static void addReadingUser(Pipe* pipe, int size, char* text,uint64_t pid) {
    PipeUser* newUser = (PipeUser*)mallocFun(sizeof(PipeUser));
    if(pipe->firstReadWaitingList==NULL){
        pipe->firstReadWaitingList=newUser;
    }else{
        pipe->lastReadWaitingList->nextUser = newUser;
    }
    pipe->lastReadWaitingList = newUser;
    newUser->nextUser = NULL;
    newUser->pid=pid;
    while(pipe->bufferWriten==0){
        block(pid);
    }
    readFromBuffer(pipe,size,text,pid);
}


static void addWritingUser(Pipe* pipe, int size, char* text,uint64_t pid) {    
    PipeUser* newUser = (PipeUser*)mallocFun(sizeof(PipeUser));
    if(pipe->firstWriteWaitingList==NULL){
        pipe->firstWriteWaitingList=newUser;
    }else{
        pipe->lastWriteWaitingList->nextUser = newUser;
    }
    pipe->lastWriteWaitingList = newUser;
    newUser->nextUser = NULL;
    newUser->pid=pid;
    while(pipe->bufferWriten==BUFF_SIZE){
        block(pid);
    }
    writeInBuffer(pipe,size,text,pid);
}

int printPipe(char* buf, Pipe* pipe) {
    int i = 0;
    while (pipePhrase1[i] != '\0') {
        *(buf++) = pipePhrase1[i++];
    }
    
    i += numToStr(pipe->pipeId, buf);
    int c = 0;
    if (pipe->writen) {
        while (pipePhrase2[c] != '\0') {
            *(buf++) = pipePhrase1[c++];
        }
    } else {
        while (pipePhrase3[c] != '\0') {
            *(buf++) = pipePhrase1[c++];
        }
    }
    *(buf++)='\n';
    i++;
    return c + i;
}