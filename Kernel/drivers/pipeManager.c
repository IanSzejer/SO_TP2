#include "../include/pipeManager.h"
//#include "scheduler.h" INCLUIRLO CUANDO LO META CON EL RESTO DEL CODIGO
#define INIT_BUFF_SIZE 1024
#define FD_AMOUNT 256
#define PIPE_AMOUNT 20

typedef unsigned long long uint64_t;
// Estructura que tendra un pipe

typedef struct {
    PipeUser* nextUser;
    char* processBuffer;
    int bufferSize;
} PipeUser;

typedef struct pipe {
    char* buffer;
    char* writePointer;
    char* readPointer;
    int bufferWriten;
    int bufferSize;
    uint64_t pipeId;
    uint64_t pipeReadRef;
    uint64_t pipeWriteRef;
    PipeUser* firstWriteWaitingList;
    PipeUser* lastWriteWaitingList;
    PipeUser* firstReadWaitingList;
    PipeUser* lastReadWaitingList;
    Pipe* nextPipe;

    int writen;
} Pipe;

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
    newPipe->buffer = (char*)mallocFun(INIT_BUFF_SIZE);
    if (newPipe->buffer == NULL)
        return -1;
    newPipe->bufferWriten = 0;
    newPipe->bufferSize = INIT_BUFF_SIZE;
    newPipe->readPointer = newPipe->buffer;
    newPipe->writePointer = newPipe->buffer;
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
        addUser(i, size, text);
    } else {
        // Como el array es circular, copio hasta el final, incremento el text y copio el resto
        if (size > pipeArray[i]) {
            memcpy(pipeArray[i]->writePointer, text, (pipeArray[i]->bufferSize - pipeArray[i]->bufferWriten) * sizeof(char));
            int amountCopied = pipeArray[i]->bufferSize - pipeArray[i]->bufferWriten;
            text += amountCopied;
            pipeArray[i]->writePointer = pipeArray[i]->buffer; // Pongo el writer al principio del buffer de nuevo
            pipeArray[i]->bufferWriten = 0;
            // Copio el resto del tamaño
            memcpy(pipeArray[i]->writePointer, text, (pipeArray[i]->bufferSize - amountCopied) * sizeof(char));
            amountCopied = pipeArray[i]->bufferSize - amountCopied;
            text += amountCopied;
            pipeArray[i]->writePointer += amountCopied;
            pipeArray[i]->bufferWriten = amountCopied;
            pipeArray[i]->writen = 1;
            size -= pipeArray[i]->bufferSize;
            addUser(i, size, text);

        } else {
            if (size < pipeArray[i]->bufferSize - pipeArray[i]->bufferWriten) {
                memcpy(pipeArray[i]->writePointer, text, (size * sizeof(char)));
                pipeArray[i]->writePointer += size;
                pipeArray[i]->bufferWriten += size;
            } else {
                memcpy(pipeArray[i]->writePointer, text, (pipeArray[i]->bufferSize - pipeArray[i]->bufferWriten) * sizeof(char));
                int amountCopied = pipeArray[i]->bufferSize - pipeArray[i]->bufferWriten;
                text += amountCopied;
                pipeArray[i]->writePointer = pipeArray[i]->buffer;
                pipeArray[i]->bufferWriten = 0;
                memcpy(pipeArray[i]->writePointer, text, (size - amountCopied) * sizeof(char)); // Copio de size, menos lo ya copiado
                amountCopied = size - amountCopied;
                pipeArray[i]->bufferWriten += amountCopied;
                pipeArray[i]->writePointer += amountCopied;
            }
        }
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

void addUser(int index, int size, char* text) {
    PipeUser* newUser = (PipeUser*)mallocFun(sizeof(PipeUser));
    pipeArray[index]->lastWriteWaitingList->nextUser = newUser;
    pipeArray[index]->lastWriteWaitingList = newUser;
    newUser->bufferSize = size;
    newUser->processBuffer = text;
    newUser->nextUser = NULL;
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