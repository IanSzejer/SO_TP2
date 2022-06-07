#ifndef MEM_DRIVER
#define MEM_DRIVER
#include "../include/lib.h"

void initMemManager(uint64_t heapInitialPosition,uint64_t maxSizeHeap);
void * mallocFun(uint64_t nbytes);
void freeFun(void * block);
void consult(char* buf);

#endif