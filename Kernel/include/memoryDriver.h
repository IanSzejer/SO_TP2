#ifndef MEM_DRIVER
#define MEM_DRIVER
#include "../include/lib.h"

void * mallocFun(unsigned nbytes);
void freeFun(void * block);
void consult(char* buf);

#endif