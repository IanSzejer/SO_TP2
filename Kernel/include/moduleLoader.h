#ifndef MODULELOADER_H
#define MODULELOADER_H

#include <stdint.h>
#include <lib.h>

void loadModules(void * payloadStart, void ** moduleTargetAddress);

#endif