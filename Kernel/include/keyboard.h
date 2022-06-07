#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "chardefs.h"
#include "lib.h"
#include <videoD.h>
#include "scheduler.h"

char* kb_read();

void keyboard_handler();


int readFromKeyboard(char * buf, uint64_t count, int ascii);
void set_kb_target(int * var);

void overwriteRegs();

#endif