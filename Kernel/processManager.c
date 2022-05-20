#include "lib.h"
/*
static
#define DEFAULT_PRIO 0
#define TICKS_CANT 5
#define MAX_LEN_NAME 256

typedef struct{
    char name[MAX_LEN_NAME];
    int id;
    int priority;
    sp;
    bp;
    foreground;
    bool block; 
}Process

Process[] processArray;

● Crear un proceso. DEBERÁ soportar el pasaje de parámetros.
● Obtener el ID del proceso que llama.
● Listar todos los procesos: nombre, ID, prioridad, stack y base pointer, foreground y
cualquier otra variable que consideren necesaria.
● Matar un proceso arbitrario.
● Modificar la prioridad de un proceso arbitrario.
● Bloquear y desbloquear un proceso arbitrario.
● Renunciar al CPU

char getId(Process process){
    return process->id;
}

void blockUnblock(){
    int id=rand();
    Process p = getProcessWithId(id)
    if(p.block){
        unblock(id)
    }else{
        block(id)
    }
}

void killRandProcess(){
    int id = rand();
    killProcess(id);
}

Process* createProcess(char* path){
    Process* new=malloc(sizeof(process))
    new->id=getFreePID();
    p->name = path;
    p->id = newPID;
    p->block = false; //arranca desbloqueado
    processArray.push(p);
    putReadyQueue(process)
}


void changeProcess(Process* actualProcess){
    saveContext(actualProcess);
    char nextPID=loadNextPID(actualProcess);
    actualProcess=loadProcess(nextPID);

}

void deleteProcess(Process* actualProcess){
    free(actualProcess)
}

void removeReadyQueue(Process* actualProcess){}


void nice(Process* actualProcess,char newPrio){}

void scheduler(){
    Process actualProcess;
    while(1){
    
    long tick=ticks();
    long aux;
    while ((aux=ticks())<tick+TICKS_CANT && processRunning(actualProcess)){
        run(actualProcess);
    }
    changeProcess(actualProcess);

    
    
    
    
    }
}

*/
