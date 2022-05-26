#ifndef LIB_H
#define LIB_H

#include <stdint.h>

#include <stddef.h>

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

int hexaToString(int n,char* buf);

extern uint64_t * getStack();


#endif