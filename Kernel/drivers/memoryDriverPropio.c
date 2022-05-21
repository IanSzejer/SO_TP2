#include "../include/lib.h"

#define INITIAL_POSITION 0x1000000
#define MAX_POSITION 10000


typedef struct memoryNode {
    uint64_t totalSpace;
    uint64_t freeSpace;
    struct memoryNode* prevNode;
    struct memoryNode* nextNode;
    uint64_t startingPointerDir;
} MemoryNode;


typedef MemoryNode* MemoryNodePtr;

int copyAnswer(char* phrase, long memoryNum, char* buf);

static MemoryNode* firstNode = NULL;

static char* infoTotalMessage = "La memoria total es: ";
static char* infoLibreMessage = ", la memoria libre es: ";
static char* infoOcupadaMessage = ", la memoria ocupada es: ";

void* mallocFun(unsigned nbytes) {
    if (firstNode == NULL) {

        firstNode =(MemoryNodePtr) INITIAL_POSITION;
        firstNode->freeSpace = MAX_POSITION - INITIAL_POSITION - sizeof(MemoryNode);
        firstNode->totalSpace = firstNode->freeSpace;
        firstNode->prevNode = NULL;
        firstNode->nextNode = NULL;
        firstNode->startingPointerDir =(uint64_t) firstNode + sizeof(MemoryNode);
    }

    MemoryNodePtr currentNode = firstNode;
    while (nbytes > currentNode->freeSpace - sizeof(MemoryNode) && currentNode != NULL) {

        currentNode = currentNode->nextNode;
    }
    if (currentNode == NULL) { // Significa que no hay espacio libre suficiente
        return NULL;
    }
    if (currentNode->freeSpace == currentNode->totalSpace) { // Significa que el nodo no esta ocupado
        currentNode->freeSpace -= nbytes;
        return (void*) currentNode->startingPointerDir;
    }
    // Sino creo un nodo nuevo que arranque a partir del espacio libre del anterior
    MemoryNodePtr newNode =(MemoryNodePtr) currentNode->startingPointerDir + (currentNode->totalSpace - currentNode->freeSpace);
    // Lo vinculo a la cadena
    newNode->prevNode = currentNode;
    newNode->nextNode = currentNode->nextNode;
    currentNode->nextNode = newNode;
    if (newNode->nextNode != NULL)
        newNode->nextNode->prevNode = newNode; // Conecto el previous del siguiente nodo al new Node

    newNode->totalSpace = currentNode->freeSpace - sizeof(MemoryNode);
    newNode->freeSpace = newNode->totalSpace - nbytes;

    currentNode->totalSpace -= currentNode->freeSpace; // Le decremento el espacio robado al current
    currentNode->freeSpace = 0;

    newNode->startingPointerDir =(uint64_t) newNode + sizeof(MemoryNode);
    return (void*) newNode->startingPointerDir;
}

void freeFun(void* block) {
    MemoryNodePtr current = firstNode;
    while (current->startingPointerDir !=(uint64_t) block && current != NULL) {
        current = current->nextNode;
    }
    // No encontro el bloque o no hay bloques reservados
    if (current == NULL) {
        return;
    }
    uint64_t nodeTotalSize = current->totalSpace + sizeof(MemoryNode);
    // Sumo el espacio al nodo anterior(con el que fusiono mi bloque)
    MemoryNodePtr prevCurrentNode = current->prevNode;
    prevCurrentNode->totalSpace += nodeTotalSize;
    prevCurrentNode->freeSpace += nodeTotalSize;
    // Conecto el nodo anterior al nodo proximo del current para rearmar la cadena
    prevCurrentNode->nextNode = current->nextNode;
    if (current->nextNode != NULL)
        current->nextNode->prevNode = prevCurrentNode;
}

void consult(char* buf) {
    if (buf == NULL){
        *buf=0;
        return;
    }
    MemoryNodePtr currentNode=firstNode;
    long freeMemory = 0;
    long occupiedMemory;
    long totalMemory = MAX_POSITION - INITIAL_POSITION;
    // Counting the node struct as occupied memory
    while (currentNode != NULL) {
        freeMemory += currentNode->freeSpace;
        currentNode = currentNode->nextNode;
    }

    occupiedMemory = totalMemory - freeMemory;

    buf += copyAnswer(infoTotalMessage, totalMemory, buf);
    buf += copyAnswer(infoLibreMessage, freeMemory, buf);
    buf += copyAnswer(infoOcupadaMessage, occupiedMemory, buf);
    *(buf++) = '\0';
}

int copyAnswer(char* phrase, long memoryNum, char* buf) {
    int c = 0;
    while (*phrase != '\0') {
        *(buf++) = *(phrase++);
        c++;
    }
    c += numToStr(memoryNum, buf);
    return c;
}