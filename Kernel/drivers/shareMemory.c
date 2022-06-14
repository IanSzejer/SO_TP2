#include "../include/lib.h"
#include "memoryDriver.h"
#include "sharedMemory.h"
#define MEM_BASIC_SIZE 1000
typedef struct memoryShared{
    uint64_t id;
    void* memRef;
}MemoryShared;

static MemoryShared sharedMemoryArray[10];
static int memorSharedindex=0;
 

void * getSharedMemory(uint64_t id){
    for(int i=0; i<memorSharedindex;i++){
        if(sharedMemoryArray[i].id==id)
            return sharedMemoryArray[i].memRef;
    }
    void* memoryPointer = mallocFun(MEM_BASIC_SIZE);
    if(memoryPointer==NULL)
        return NULL;
    sharedMemoryArray[memorSharedindex].id=id;
    sharedMemoryArray[memorSharedindex].memRef=memoryPointer;
    memorSharedindex++;
    return memoryPointer;
}