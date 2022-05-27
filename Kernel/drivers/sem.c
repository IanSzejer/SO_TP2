#include "../include/sem.h"
#include "../include/scheduler.h"
#include "../include/lib.h"
#include "../include/memoryDriverPropio.h"

static semaphore_t semSpaces[MAX_SEM];
static char* semPrint;

static void initSems();
static uint64_t findAvailableSpace();
static uint64_t lockSem; // Para bloquear al momento de un open o close de cualquier semaforo.
static uint64_t findSem(char *name);
static uint64_t enqeueProcess(uint64_t pid, sem_t *sem);
static uint64_t dequeueProcess(sem_t *sem);
int printSem(char* buf,sem_t sem);
void printProcessesBlocked(process_t *process);


static void initSems()
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


int createSem(char *semName, uint64_t initValue)
{
    if(semSpaces == NULL){
        initSems();
    }
    int pos;
    //busco un lugar dispo y lo guardo
    if ((pos = findAvailableSpace()) != -1)
        initializeSem(initValue, pos, semName);

    //devuelvo donde lo guarde
    return pos;
}


static uint64_t enqeueProcess(uint64_t pid, sem_t *sem)
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

static uint64_t dequeueProcess(sem_t *sem)
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


uint64_t semOpen(char *name, uint64_t initValue)
{
    while (_xchg(&lockSem, 1) != 0) // esperando a que el lock este disponible
        ;
    int semIndex = findSem(name);
    if (semIndex == -1) 
    {
        semIndex = createSem(name, initValue);
        if (semIndex == -1)
        {
            _xchg(&lockSem, 0);
            return -1; 
        }
    }
    semSpaces[semIndex].sem.size++;
    _xchg(&lockSem, 0);
    return semIndex; 
}

uint64_t semClose(char *name)
{
    while (_xchg(&lockSem, 1) != 0)
        ;
    int semIndex = findSem(name);
    if (semIndex == -1)
    {
        return -1; 
    }
    if ((--semSpaces[semIndex].sem.size) <= 0)
        semSpaces[semIndex].available = TRUE;
    _xchg(&lockSem, 0);
    return 1;
}

uint64_t semWait(char* semName)
{
    uint64_t semIndex=findSem(semName);
    if (semIndex >= MAX_SEM)
        return -1;
    sem_t *sem = &semSpaces[semIndex].sem;

    while (_xchg(&sem->lock, 1) != 0)
        ;

    if (sem->value > 0)
    {
        sem->value--;
        _xchg(&sem->lock, 0);
    }
    else
    {
        uint64_t pid = getPid();
        if (enqeueProcess(pid, sem) == -1)
        {
            _xchg(&sem->lock, 0);
            return -1;
        }

        _xchg(&sem->lock, 0);
        if (block(pid) == -1)
        {
            return -1;
        }
        sem->value--;
    }
    return 0;
}

uint64_t semPost(char* semName)
{
    uint64_t semIndex=findSem(semName);
    if (semIndex == -1)
    {
        return -1;
    }

    sem_t *sem = &semSpaces[semIndex].sem;
    while (_xchg(&sem->lock, 1) != 0)
        ;
    sem->value++;
    int pid = 0;
    if (sem->sizeList > 0)
    {
        if ((pid = dequeueProcess(sem)) == -1)
        {
            _xchg(&sem->lock, 0);
            return -1;
        }
    }
    _xchg(&sem->lock, 0);
    unblock(pid) ? : forceTimer();
    return 0;
}


static uint64_t findSem(char *name)
{
    for (int i = 0; i < MAX_SEM; i++)
    {
        if (semSpaces[i].available == FALSE && strcmp(name, semSpaces[i].sem.name))
        {
            return i;
        }
    }
    return -1;
}

static int copyText(char* buf,char* text){
    int i=0;
    while(text[i]!='\0'){
        *(buf++)=text[i];
        i++;
    }
    return i;

}

void sem(char* buf)
{
    char semText[] = ("SEM'S NAME\t\tSTATE\t\tBLOCKED PROCESSES\n");
    buf+= copyText(buf,semText);
    for (int i = 0; i < MAX_SEM; i++)
    {
        int toPrint = !(semSpaces[i].available);
        if (toPrint)
        {
            buf+= printSem(buf,semSpaces[i].sem);
        }
    }
    buf+='\0';
}

char *getSemName(uint64_t semIndex)
{
    if (semIndex >= MAX_SEM)
    {
        print("Wrong Index in getSemName\n");
        return NULL;
    }
    return semSpaces[semIndex].sem.name;
}


void printProcessesBlocked(process_t *process)
{
    while (process != NULL)
    {
        printInt(process->pid);
        print(" ");
        process = process->next;
    }
    print("-");
}

void printProcessesSem(uint64_t semIndex)
{
    if (semIndex >= MAX_SEM)
    {
        print("Wrong Index in printProcessesSem\n");
        return;
    }
    sem_t sem = semSpaces[semIndex].sem;
    printProcessesBlocked(sem.firstProcess);
}

int printSem(char* buf,sem_t sem)
{
    int i=0;
    while(sem.name[i]!='\0'){
        *(buf++)=sem.name[i];
        i++;
    }
    if (i > 10){
        *(buf++)='\t';
        *(buf++)='\t';
        i+=2;
    }
    else{
        *(buf++)='\t';
        *(buf++)='\t';
        *(buf++)='\t';
        *(buf++)='\t';
        i+=4;
    }
    int c=0;
    c += numToStr(sem.value,buf);
    buf+=c;
    *(buf++)='\t';
    *(buf++)='\t';
    *(buf++)='\t';
    c+=3;
    int j=0;
    if (sem.firstProcess!=NULL)
        j+= numToStr(sem.firstProcess->pid,buf);
    *(buf++)='\n';
    j++;
    return i+c+j;
}
