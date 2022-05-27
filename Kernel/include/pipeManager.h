#ifndef PIPE
#define PIPE
#include "../include/memoryDriverPropio.h"
#include "../include/scheduler.h"
#include "lib.h"

int pipeFun(int pipeFd[2], int processId);
int writeInPipe(uint64_t writePipeId, uint64_t pid, int size, char* text);
void listPipes(char* buf);
static void addUser(int index, int size, char* text);
static int getReadRef(uint64_t pipeId);
static int getWriteRef(uint64_t pipeId);


#endif
