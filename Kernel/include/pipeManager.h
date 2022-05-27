#ifndef PIPE
#define PIPE
#include "../include/memoryDriverPropio.h"
#include "sch"
#include <lib.h>
#define PIPE_AMOUNT 20
#define BUFF_SIZE 1024
#define FD_AMOUNT 256

typedef struct {
    uint64_t pid;
    PipeUser* nextUser;
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
    Pipe* nextPipe;

    int writen;
} Pipe;


int pipeFun(int pipeFd[2], int processId);
int writeInPipe(uint64_t writePipeId, uint64_t pid, int size, char* text);
void listPipes(char* buf);
static void writeInBuffer(Pipe* pipe,uint64_t pid,uint64_t size,char* text);
static uint64_t min(uint64_t num1,uint64_t num2);
static void startreading(Pipe* pipe);
static void startWriting(Pipe* pipe);
static void addWritingUser(Pipe* pipe, int size, char* text,uint64_t pid);
static void addReadingUser(Pipe* pipe, int size, char* text,uint64_t pid); 
static int getReadRef(uint64_t pipeId);
static int getWriteRef(uint64_t pipeId);


#endif
