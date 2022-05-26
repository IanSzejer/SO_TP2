#ifndef SEM_H
#define SEM_H

#define MAX_NAME 15
#define MAX_SEM 30

#include <lib.h>

typedef struct pNode
{
    struct pNode *next;
    uint64_t pid;
} process_t;
typedef struct
{
    char name[MAX_NAME];
    process_t *firstProcess;
    process_t *lastProcess;  
    uint64_t lock;
    int value;
    uint64_t size;     
    uint64_t sizeList;
} sem_t;

typedef struct
{
    sem_t sem;
    uint64_t available;
} semaphore_t;

extern uint64_t _xchg(uint64_t *lock, int value);

void initSems();
uint64_t semOpen(char *name, uint64_t initValue);
uint64_t semClose(char *name);
uint64_t semWait(uint64_t semIndex);
uint64_t semPost(uint64_t semIndex);
void sem();
char *getSemName(uint64_t semIndex);
void printProcessesSem(uint64_t semIndex);

#endif