#ifndef _SHELL_H_
#define _SHELL_H_

#include <stdint.h>
#include "stdinout.h"
#include "colors.h"
#include "test.h"
//#include "videoColors.h"

#define SHELL_COLOR GREEN 
#define SHELLH 10
#define SHELLW 80
#define SHELL_MSG "SHELL>:$ "

#define MAX_CMD_SIZE 30

int theShell();
void sleep(int seconds);
#endif