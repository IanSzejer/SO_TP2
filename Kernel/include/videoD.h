#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>
#include "clock.h"


void ncPrint(const char * string);
void ncPrintColor(const char * string, char color);
void ncPrintChar(char character, char color);
void ncNewline();
void ncDelete();
void ncPrintDec(uint64_t value);
void ncPrintHex(uint64_t value);
void ncPrintBin(uint64_t value);
void ncPrintBase(uint64_t value, uint32_t base);
void ncClear();
int ncSplitConsole(int screens, int screen); 
int ncChangeScreen(int screen); 
void ncClearLine(); 
void ncInitVideoConsole();

#endif