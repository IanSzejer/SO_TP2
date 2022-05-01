#include "time.h"
#include <stdint.h>
#include "videoD.h"
#include "lib.h"
#include "interrupts.h"
#include "keyboard.h"

typedef void (*IRQHandlerR)(uint64_t rsp);

static void int_20();
static void int_21(uint64_t *);
static IRQHandlerR handlers[8] = {(IRQHandlerR) &int_20, (IRQHandlerR) &int_21};

void irqDispatcher(uint64_t irq, uint64_t rsp) {
	IRQHandlerR irqHandler = handlers[irq];

	if (irqHandler != 0)
		irqHandler(rsp);
}

void int_20() {
	 timer_handler();
}

void int_21(uint64_t * stack) {
	keyboardDriver(stack);
}


