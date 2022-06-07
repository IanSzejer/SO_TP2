
#include "scheduler.h"
#include "../include/lib.h"

#ifndef PIPE_H
#define PIPE_H

#define PIPE_AMOUNT 20
#define BUFF_SIZE 1024
#define FD_AMOUNT 256

typedef struct pipeUser{
    uint64_t pid;
    struct pipeUser* nextUser;
} PipeUser;

typedef struct pipe {
    char* bufferInit;
    char* bufferEnd;
    char* writePointer;
    char* readPointer;
    uint64_t bufferWriten;
    uint64_t pipeId;

    uint64_t pipeReadRef;
    uint64_t pipeWriteRef;

    PipeUser* firstWriteWaitingList;
    PipeUser* lastWriteWaitingList;
    PipeUser* firstReadWaitingList;
    PipeUser* lastReadWaitingList;
    struct pipe* nextPipe;

    int writen;
} Pipe;


int pipeFun(int pipeFd[2], int processId);
int writeInPipe(uint64_t writePipeId, uint64_t pid, int size, char* text);
int readFromPipe(uint64_t readPipeId, uint64_t pid, int size, char* text);
void listPipes(char* buf);
int printPipe(char* buf, Pipe* pipe);



#endif
