#include <stdint.h>
#include <videoD.h>
#include "lib.h"
#include "clock.h"




int BCDToDecimal(uint64_t value) {
    return ((value & 0xF0) >> 4) * 10 + (value & 0x0F);
}

unsigned int seconds() {
    return BCDToDecimal(getRTC(SECONDS));
}

unsigned int minutes() {
    return BCDToDecimal(getRTC(MINUTES));
}

unsigned int hours() {
    return BCDToDecimal(getRTC(HOURS))+TIMEZONE_DIFF;
}

unsigned int day() {
    return BCDToDecimal(getRTC(DAY));
}

unsigned int month() {
    return BCDToDecimal(getRTC(MONTH));
}

unsigned int year() {
    return BCDToDecimal(getRTC(YEAR));
}





