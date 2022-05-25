#include "../include/pipeManager.h"
//#include "scheduler.h" INCLUIRLO CUANDO LO META CON EL RESTO DEL CODIGO
#define INIT_BUFF_SIZE 1024
#define FD_AMOUNT 256
#define PIPE_AMOUNT 20
typedef unsigned long long uint64_t;
//Estructura que tendra un pipe


typedef struct{
    PipeUser* nextUser;
    char* processBuffer;
    int bufferSize;
}PipeUser;

typedef struct pipe{
    char* buffer;
    char* writePointer;
    char* readPointer;
    int bufferWriten;
    int bufferSize;
    uint64_t pipeId;
    uint64_t pipeReadRef;
    uint64_t pipeWriteRef;
    PipeUser* firstWriteWaitingList;
    PipeUser* lastWriteWaitingList;
    PipeUser* firstReadWaitingList;
    PipeUser* lastReadWaitingList;
    Pipe* nextPipe;

    int writen;
}Pipe;

static Pipe* pipeArray[PIPE_AMOUNT]={0};


//void startFdOccupied(PipeList* list);

void addUser(int index,int size,char* text);
int getReadRef(uint64_t pipeId);
int getWriteRef(uint64_t pipeId);



//static PipeList* list=NULL;
static uint64_t pipeId=1;
uint64_t getPipeId(){
    return pipeId++;
}

/*Funcion que se encarga de crear el pipe y devolver los fd
**  Recibe: array de 2 int donde guardara los fd creados
**  Return: -1 por error y 0 si se pudieron crear correctamente
*/
int pipeFun(int pipeFd[2], int processFd){

    //Creo lista en caso de que no este creada todavia
    if (list==NULL){
        list=(PipeList*) mallocFun(sizeof(PipeList));
        if (list==NULL)
            return -1;
        list->pipeAmount=0;
        list->firstPipe=NULL;
        list->lastPipe=NULL;
        startFdOccupied(list);
    }
    Pipe* newPipe = (Pipe*) mallocFun(sizeof(Pipe));
    if (newPipe==NULL)
        return -1;
    //Creo el pipe completo
    newPipe->buffer = (char*) mallocFun(INIT_BUFF_SIZE);
    if (newPipe->buffer==NULL)
        return -1;
    newPipe->bufferWriten=0;
    newPipe->bufferSize=INIT_BUFF_SIZE;
    newPipe->readPointer=newPipe->buffer;
    newPipe->writePointer=newPipe->buffer;
    newPipe->firstReadWaitingList=NULL;
    newPipe->firstWriteWaitingList=NULL;
    newPipe->lastReadWaitingList=NULL;
    newPipe->lastWriteWaitingList=NULL;
    newPipe->writen=0;
    newPipe->pipeId=getPipeId();
    newPipe->pipeReadRef=getReadRef(newPipe->pipeId);
    newPipe->pipeWriteRef=getWriteRef(newPipe->pipeId);
    newPipe->nextPipe=NULL;

    //Si es el primero lo pongo de first y last, sino solo de last y lo conecto
    if (list->lastPipe==NULL){
        list->firstPipe=newPipe;
        list->lastPipe=newPipe;
    }else{
        list->lastPipe->nextPipe=newPipe;
        list->lastPipe=newPipe;
    }
    
    return 0;
}

//Los ref arrancan en 100 y son 100 para read y 101 para write x ejemplo
int getReadRef(uint64_t pipeId){
    return pipeId*2+100;
}
int getWriteRef(uint64_t pipeId){
    return pipeId*2+101;
}


//0 para ocupado, 1 para libre
void startFdOccupied(PipeList* list){
    list->fdOccupied[0]=0;
    list->fdOccupied[1]=0;
    list->fdOccupied[2]=0;
    for(int i = 3; i<FD_AMOUNT;i++){
        list->fdOccupied[i]=1;
    };
}



int writeInPipe(uint64_t writePipeId,uint64_t pid,int size,char* text){
    int found=0;
    int i=0;
    for(; i<PIPE_AMOUNT && !found;i++){
        if(pipeArray[i]!=NULL && pipeArray[i]->pipeId==writePipeId)
            found=1;
    }
    if(found==0){
        return -1;
    }
    if (pipeArray[i]->writen==1){
        addUser(i,size,text);
    }else{
        //Como el array es circular, copio hasta el final, incremento el text y copio el resto
        if(size>pipeArray[i]){
            memcpy(pipeArray[i]->writePointer,text,(pipeArray[i]->bufferSize-pipeArray[i]->bufferWriten)*sizeof(char));
            int amountCopied=pipeArray[i]->bufferSize-pipeArray[i]->bufferWriten;
            text+=amountCopied;
            pipeArray[i]->writePointer=pipeArray[i]->buffer;    //Pongo el writer al principio del buffer de nuevo
            pipeArray[i]->bufferWriten=0;
            //Copio el resto del tamaño
            memcpy(pipeArray[i]->writePointer,text,(pipeArray[i]->bufferSize-amountCopied)*sizeof(char));
            amountCopied=pipeArray[i]->bufferSize-amountCopied;
            text+=amountCopied;
            pipeArray[i]->writePointer+=amountCopied;
            pipeArray[i]->bufferWriten=amountCopied;
            pipeArray[i]->writen=1;
            size-=pipeArray[i]->bufferSize;
            addUser(i,size,text);

        }else{
            if(size<pipeArray[i]->bufferSize-pipeArray[i]->bufferWriten){
                memcpy(pipeArray[i]->writePointer,text,(size*sizeof(char)));
                pipeArray[i]->writePointer+=size;
                pipeArray[i]->bufferWriten+=size;
            }
            else{
                memcpy(pipeArray[i]->writePointer,text,(pipeArray[i]->bufferSize-pipeArray[i]->bufferWriten)*sizeof(char));
                int amountCopied=pipeArray[i]->bufferSize-pipeArray[i]->bufferWriten;
                text+=amountCopied;
                pipeArray[i]->writePointer=pipeArray[i]->buffer; 
                pipeArray[i]->bufferWriten=0;
                memcpy(pipeArray[i]->writePointer,text,(size-amountCopied)*sizeof(char));//Copio de size, menos lo ya copiado
                amountCopied=size-amountCopied;
                pipeArray[i]->bufferWriten+=amountCopied;
                pipeArray[i]->writePointer+=amountCopied;            
            }

        }
    }


    void addUser(int index,int size,char* text){
        PipeUser* newUser=(PipeUser*)mallocFun(sizeof(PipeUser));
        pipeArray[i]->lastWriteWaitingList->nextUser=newUser;
        pipeArray[i]->lastWriteWaitingList=newUser;
        newUser->bufferSize=size;
        newUser->processBuffer=text;
        newUser->nextUser=NULL;
    }
        
}