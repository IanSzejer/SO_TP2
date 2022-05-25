
#include "../include/sem.h"

static semaphore_t semaphores[MAX_SEM];


void initSems()
{
    for (int i = 0; i < MAX_SEM; i++){
        semSpaces[i].available = TRUE;
    }
}

static uint64_t findAvailableSpace()
{
    for (int i = 0; i < MAX_SEM; i++)
    {
        if (semSpaces[i].available == TRUE)
        {
            semSpaces[i].available = FALSE;
            return i;
        }
    }
    return -1; // No hay mas espacio en el vector para crear otro semaforo
}

static void initializeSem(uint64_t initValue, int pos, char *semName){
    memcpy(semSpaces[pos].sem.name, semName, strlen(semName));
    semSpaces[pos].sem.value = initValue;
    semSpaces[pos].sem.lock = 0; // Inicialmente no esta lockeado.
    semSpaces[pos].sem.firstProcess = NULL;
    semSpaces[pos].sem.lastProcess = semSpaces[pos].sem.firstProcess;
    semSpaces[pos].sem.size = 0;
    semSpaces[pos].sem.sizeList = 0;
}


static int createSem(char *semName, uint64_t initValue)
{
    int pos;
    //busco un lugar dispo y lo guardo
    if ((pos = findAvailableSpace()) != -1)
        initializeSem(initValue, pos, semName);

    //devuelvo donde lo guarde
    return pos;
}


uint64_t enqeueProcess(uint64_t pid, sem_t *sem)
{
    process_t *process = mallocFun(sizeof(process_t));
    if (process == NULL)
    {
        return -1;
    }
    process->pid = pid;
    if (sem->sizeList == 0)
    {
        sem->firstProcess = process;
        sem->lastProcess = sem->firstProcess;
        process->next = NULL;
    }
    else
    {
        sem->lastProcess->next = process;
        process->next = NULL;
        sem->lastProcess = process;
    }
    sem->sizeList++;
    return 0;
}

uint64_t dequeueProcess(sem_t *sem)
{
    if (sem == NULL || sem->firstProcess == NULL)
        return -1;
    process_t *current = sem->firstProcess;
    int pid = current->pid;
    sem->firstProcess = current->next;
    if (sem->firstProcess == NULL)
    {
        sem->lastProcess = NULL;
    }
    freeFun(current);
    sem->sizeList--;
    return pid;
}


uint64_t semWait(sem_t * sem){

}