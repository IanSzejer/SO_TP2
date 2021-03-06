#ifdef BUDDY

#include "memoryDriver.h"

#define FREE 0
#define OCCUPIED 1

#define HEADER_SIZE 8
#define MIN_ALLOC_LOG2 6                     // 64 bytes
#define MAX_ALLOC_LOG2 (30 - MIN_ALLOC_LOG2) // 1Gb max alloc
#define BIN_POW(x) (1 << (x))


typedef struct list_t
{
    uint64_t occupied;
    uint64_t bucketLevel;
    struct list_t *prev, *next;
} list_t;

static list_t buckets[MAX_ALLOC_LOG2];
static int bucketSize;
static list_t *base=NULL;
static int baseSize;
static int availableMem;

static void addBucket(list_t *list, list_t *entry, uint64_t level);
static uint64_t getBucketLevel(uint64_t bytes);
static uint64_t getFreeBucketLevel(uint64_t bucketLevel);
static list_t *getBuddy(list_t *node);
static uint64_t log2(uint64_t n);
static uint64_t isPowerOfTwo(int n);
static list_t *getAdress(list_t *node);
static int copyAnswer(char* phrase, long memoryNum, char* buf);
static list_t *listPop(list_t *list);
static void listInit(list_t *list);
static void listRemove(list_t *entry);
static void listPush(list_t *list, list_t *entry);
static int isEmpty(list_t *list);

void initMemManager(uint64_t heapInitialPosition,uint64_t maxSizeHeap)
{
    base = (list_t *) heapInitialPosition ;
    baseSize = maxSizeHeap;
    availableMem = baseSize;
    bucketSize = (int)log2(baseSize) - MIN_ALLOC_LOG2 + 1;

    if (bucketSize > maxSizeHeap)
    {
        bucketSize = maxSizeHeap;
    }

    for (int i = 0; i < bucketSize; i++)
    {
        listInit(&buckets[i]);
        buckets[i].bucketLevel = i;
    }
    addBucket(&buckets[bucketSize - 1], base, bucketSize - 1);
}

void *mallocFun(uint64_t nbytes)
{
    if (nbytes == 0)
        return NULL;

    uint64_t totalSize;
    if ((totalSize = nbytes + (sizeof(list_t) * 2)) > baseSize) // ...
        return NULL;

    uint64_t bucketLevel = getBucketLevel(totalSize);
    uint64_t freeBucketLevel;
    if ((freeBucketLevel = getFreeBucketLevel(bucketLevel)) == -1)
    {
        return NULL;
    }
    list_t *node;
    for (node = listPop(&buckets[freeBucketLevel]); bucketLevel < freeBucketLevel; freeBucketLevel--)
    {
        node->bucketLevel--;
        addBucket(&buckets[freeBucketLevel - 1], getBuddy(node), freeBucketLevel - 1);
    }
    node->occupied = OCCUPIED;
    availableMem -= BIN_POW(MIN_ALLOC_LOG2 + bucketLevel);
    return (void *)++node;
}

void freeFun(void *block)
{
    if (block == NULL) 
        return;

    list_t *list = (list_t *)block - 1;
    list->occupied = FREE;
    availableMem += BIN_POW(MIN_ALLOC_LOG2 + list->bucketLevel);

    list_t *buddy = getBuddy(list);
    while (list->bucketLevel != bucketSize - 1 && buddy->bucketLevel == list->bucketLevel && !buddy->occupied)
    {
        listRemove(buddy);
        list = getAdress(list);
        list->bucketLevel++;
        buddy = getBuddy(list);
    }
    listPush(&buckets[list->bucketLevel], list);
}

void consult(char* buf)
{
    buf+=copyAnswer("Total Memory: ",baseSize,buf);
    buf+=copyAnswer("Available Memory: ",availableMem,buf);
    buf+=copyAnswer("Used Memory: ",baseSize - availableMem,buf);
    *(buf++) = '\0';
}

static int copyAnswer(char* phrase, long memoryNum, char* buf) {
    int c = 0;
    while (*phrase != '\0') {
        *(buf++) = *(phrase++);
        c++;
    }
    c += numToStr(memoryNum, buf);
    return c;
}

static void addBucket(list_t *list, list_t *entry, uint64_t level)
{
    entry->bucketLevel = level;
    entry->occupied = FREE;
    listPush(list, entry);
}

static uint64_t log2(uint64_t n)
{
    if (n == 0)
    {
        return -1;
    }
    int logValue = -1;
    while (n)
    {
        logValue++;
        n >>= 1;
    }
    return logValue;
}

static uint64_t isPowerOfTwo(int n)
{
    if (n == 0)
        return 0;
    while (n != 1)
    {
        if (n % 2 != 0)
            return 0;
        n = n / 2;
    }
    return 1;
}

static uint64_t getBucketLevel(uint64_t bytes)
{
    uint64_t n = log2(bytes);
    if (n < MIN_ALLOC_LOG2)
        return 0;
    n -= MIN_ALLOC_LOG2;
    return (isPowerOfTwo(n) ? n : n + 1);
}

static uint64_t getFreeBucketLevel(uint64_t bucketLevel)
{
    uint64_t level = bucketLevel;
    while (level < bucketSize && isEmpty(&buckets[level]))
        level++;
    return (level >= bucketSize) ? -1 : level;
}

static list_t *getBuddy(list_t *node)
{
    uint64_t currentOffset = (uint64_t)node - (uint64_t)base;
    uint64_t newOffset = currentOffset ^ BIN_POW(MIN_ALLOC_LOG2 + node->bucketLevel);
    return (list_t *)(newOffset + base);
}

static list_t *getAdress(list_t *node)
{
    uint64_t mask = BIN_POW(MIN_ALLOC_LOG2 + node->bucketLevel);
    mask = ~mask;

    uint64_t currentOffset = (uint64_t)node - (uint64_t)base;
    uint64_t newOffset = currentOffset & mask;
    return (list_t *)((uint64_t)base + newOffset);
}


// List methods
static void listInit(list_t *list)
{
    list->prev = list;
    list->next = list;
    list->occupied = FREE;
}

static void listPush(list_t *list, list_t *entry)
{
    list_t *prev = list->prev;
    entry->prev = prev;
    entry->next = list;
    prev->next = entry;
    list->prev = entry;
}

static void listRemove(list_t *entry)
{
    list_t *prev = entry->prev;
    list_t *next = entry->next;
    prev->next = next;
    next->prev = prev;
}

static list_t *listPop(list_t *list)
{
    list_t *back = list->prev;
    if (back == list)
        return NULL;
    listRemove(back);
    return back;
}

static int isEmpty(list_t *list)
{
    return list->prev==list;
}

#endif