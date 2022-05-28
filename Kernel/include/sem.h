#include "lib.h"
#ifndef SEM_H
#define SEM_H

#define MAX_NAME 15
#define MAX_SEM 30
#define TRUE 1
#define FALSE 0


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
extern void forceTimer();

int createSem(char *semName, uint64_t initValue);

int semOpen(char *name, uint64_t initValue);
uint64_t semClose(char *name);
uint64_t semWait(char* semName);
uint64_t semPost(char* semName);
void sem(char* buf);
char *getSemName(uint64_t semIndex);
int printSem(char* buf,sem_t sem);

#endif