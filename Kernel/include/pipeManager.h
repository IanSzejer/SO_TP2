#ifndef PIPE
#define PIPE
#include "../include/memoryDriverPropio.h"
#include "../include/scheduler.h"
#include "lib.h"

static void addUser(int index, int size, char* text);
static int getReadRef(uint64_t pipeId);
static int getWriteRef(uint64_t pipeId);



#endif
