#ifndef PIPE
#define PIPE
#include "../include/memoryDriverPropio.h"
#include "../include/scheduler.h"

void addUser(int index,int size,char* text);
int getReadRef(uint64_t pipeId);
int getWriteRef(uint64_t pipeId);



#endif
