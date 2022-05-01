#include "time.h"


static unsigned long ticks = 0;
void (*routine)() = 0;

void timer_handler() {
	ticks++;
}

void set_routine(void (*f)()) {
	routine = f;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}
