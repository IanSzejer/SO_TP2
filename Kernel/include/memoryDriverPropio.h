#include "../include/lib.h"
#ifndef MEM_DRIVER_PROPIO
#define MEM_DRIVER_PROPIO

void * mallocFun(uint64_t nbytes);
void freeFun(void * block);
void consult(char* buf);

#endif