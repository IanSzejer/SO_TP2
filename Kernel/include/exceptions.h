
#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_
#include <stdio.h>
#include <videoD.h>
#include <stdint.h>
#include <clock.h>
#include <lib.h>

void setExceptionRebootPoint(uint64_t ip, uint64_t * sp);

#endif