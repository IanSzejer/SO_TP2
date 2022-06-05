#ifndef LIB_H
#define LIB_H

#include <stddef.h>

#ifdef __INT8_TYPE__
typedef __INT8_TYPE__ int8_t;
#endif
#ifdef __INT16_TYPE__
typedef __INT16_TYPE__ int16_t;
#endif
#ifdef __INT32_TYPE__
typedef __INT32_TYPE__ int32_t;
#endif
#ifdef __INT64_TYPE__
typedef __INT64_TYPE__ int64_t;
#endif
#ifdef __UINT8_TYPE__
typedef __UINT8_TYPE__ uint8_t;
#endif
#ifdef __UINT16_TYPE__
typedef __UINT16_TYPE__ uint16_t;
#endif
#ifdef __UINT32_TYPE__
typedef __UINT32_TYPE__ uint32_t;
#endif
#ifdef __UINT64_TYPE__
typedef __UINT64_TYPE__ uint64_t;
#endif

#define REG_COUNT 17
#define MOD_SIZE 25492
#define SAMPLECODE_PTR 0x400000
#define MAX_STR_LENGTH 300
void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpuVendor(char *result);

int getRTC(int code);

char kbReadUntilCode();

int keyboardActivated();

int getKbCode();

extern void infoReg(char ** buf);

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

void updateRegs(uint64_t* regs);
void getRegs(char ** buf);

int numToStr(int num, char * str);

int hexaToString(uint64_t n,char* buf);

extern uint64_t * getStack();
int strcmp(char *X, char *Y);
uint64_t strlen(char* text,uint64_t size);

uint64_t createContext(uint64_t stack,void* (*funcion)(void*),uint32_t argc,char arguments[6][21]);

#endif