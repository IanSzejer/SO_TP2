#ifndef _clock_
#define _clock_

#define SECONDS 0
#define MINUTES 2
#define HOURS 4
#define DAY 7
#define MONTH 8
#define YEAR 9
#define TIMEZONE_DIFF -3
#define DATETIME_LENGTH (DATE_LENGTH + 3 + TIME_LENGTH)
#define TIME_LENGTH 8
#define DATE_LENGTH 10 

int BCDToDecimal(uint64_t value);

unsigned int seconds();

unsigned int minutes();

unsigned int hours();

unsigned int day();

unsigned int month();

unsigned int year();

#endif