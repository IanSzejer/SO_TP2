#include "../include/phylo.h"
#include "../include/shell.h"
#define MAX_PHYL 15
#define MIN_PHYL 2 
#define INIT_PHYL 5
#define SEM_PHYL "semPhylos"
#define LENGTH 10
#define QUANTUM 3



typedef struct {
    uint64_t pid;
    int state;
    uint64_t semIndex;
    char semName[LENGTH];
} phylo_t;

enum state{
    THINK,
    EAT,
    WAIT
};

static phylo_t phylos[MAX_PHYL];
static int seated;
char* sem;


static int addPhylo(int pIndex);
static int removePhylo(int pIndex);
void endTable();
void phyloProcess(int argc, char **argv);
static void printState();
void putChopstick();
void takeChopstick(int pIndex);
void update(int pIndex);
int left(int pIndex);
int right(int pIndex);


void phylo(int argc, char** argv){
    print("Bienvenido a PHYLO \n");
    if(openSemaphore(SEM_PHYL,1) == -1){
        print("Error abriendo semaforo\n");
        return;
        
    }
    sem = SEM_PHYL;

    seated = 0;
    for (int i=0; i<INIT_PHYL; i++ ){
        if(addPhylo(i) == -1){
            print("Error agregando filosofo\n");
        }
    } 

    print("Presione 'a' para agregar o 'r' para eliminar un filosofo. Presione 'q' para salir\n");
    char c;
    while ((c = getCharSys()) != 0)
    {
        switch (c)
        {
        case 'a':
        case 'A':
            if (addPhylo(seated) == -1)
            {
                print("Error agregando filosofo\n");
            }
            break;
        case 'r':
        case 'R':
            if (removePhylo(seated - 1) == -1)
            {
                print("Error eliminando filosofo\n");
            }
            break;
        case 'q':
        case 'Q':
            endTable();
            if ((closeSemaphore(SEM_PHYL)) == -1)
                print("Error cerrando semaforo\n");
            return;
        }
    }
}

static int addPhylo(int pIndex){
    if(pIndex >= MAX_PHYL || pIndex<0){
        return -1;
    }

    waitSem(sem);
    seated++;
    char semName[LENGTH] = "phyl";
    numToStr(pIndex, phylos[pIndex].semName, LENGTH);
    strcat(phylos[pIndex].semName, semName);
    if((phylos[pIndex].semIndex = openSemaphore(phylos[pIndex].semName,1)) == -1){
        print("Error abriendo semaforo");
        return -1;
    }
    char currentSeated[LENGTH];
    uintToBase(seated, currentSeated, 10);
    char * argv[] = {"phylo", currentSeated};
    phylos[pIndex].state = THINK;

    if((phylos[pIndex].pid = new_process((void *(*)(void*))&phyloProcess,argv,1,"phylo")) == 0){
        print("Error creando proceso filosofo");
        return -1;
    }

    postSem(sem);
    return 0;
}

void phyloProcess(int argc, char **argv)
{
    int index = atoi(argv[1]);
    while (1)
    {
        sleep(QUANTUM);
        takeChopstick(index);
        sleep(QUANTUM);
        putChopstick(index);
    }
}

void takeChopstick(int pIndex)
{
    waitSem(sem);
    phylos[pIndex].state = WAIT;
    update(pIndex);
    postSem(sem);
    waitSem(phylos[pIndex].semName);
}

void putChopstick(int pIndex)
{
    waitSem(sem);
    phylos[pIndex].state = THINK;
    update(left(pIndex));
    update(right(pIndex));
    postSem(sem);
}

int left(int pIndex)
{
    return (pIndex == 0) ? seated - 1 : pIndex - 1;
}

int right(int pIndex)
{
    return (pIndex == seated - 1) ? 0 : pIndex + 1;
}

void update(int pIndex)
{
    if (phylos[pIndex].state == WAIT && phylos[left(pIndex)].state != EAT && phylos[right(pIndex)].state != EAT)
    {
        phylos[pIndex].state = EAT;
        printState();
        postSem(phylos[pIndex].semName);
    }
}

static int removePhylo(int pIndex){
    if(pIndex >= MAX_PHYL || pIndex <= MIN_PHYL){
        return -1;
    }

    waitSem(sem);
    seated--;
    
    int eat = (phylos[pIndex].state==EAT);

    if(closeSemaphore(phylos[pIndex].semName) == -1){
        print("Error cerrando semaforo\n");
    }
    kill(phylos[pIndex].pid); 

    if(eat){
        update(pIndex-1);
        update(0);
    }

    postSem(sem);
    return 0;
}

void endTable()
{
    while (seated > 0)
    {
        if (closeSemaphore(phylos[seated - 1].semName) == -1)
        {
            print("Error cerrando el filosofo\n");
            return;
        }
        kill(phylos[seated - 1].pid);
        seated--;
    }
}

static void printState()
{
    for (int i = 0; i < seated; i++)
    {
        (phylos[i].state == EAT) ? print(" E ") : print(" . ");
    }
    putChar('\n');
}