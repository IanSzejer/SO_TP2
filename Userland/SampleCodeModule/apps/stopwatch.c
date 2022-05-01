#include "stopwatch.h"

static long tickss;
static long startTicks;
static int lastTick;
static int stopped;;
static int started;;
static int accumTicks;



void initStopWatch() {
    tickss = startTicks = lastTick = stopped = started = accumTicks = 0;
}


void stopWatchStart() {
    int aux = ticks();
    do {
        startTicks = ticks();
    }
    while (startTicks == aux);
    lastTick = startTicks;
    stopped = 0;
    started = 1;
}

int stopWatchChanged() {
    if (!started || stopped)
        return 0;
    tickss = ticks();
    return tickss - lastTick >= 2;
}


static void twoDigitsNumToStr(int num, char * buf) {
    if (num<10) {
        buf[0] = '0';
        numToStr(num, &buf[1],10);
    }
    else {
        numToStr(num, &buf[0],10);
    }  
}

void stopWatchStop() {
    if (stopped)
        return;
    stopped = 1;
    accumTicks += ticks() - startTicks;
}

void printStopWatch() {
    
    char template[] = "00:00:00,00\n";

    if (!started) {
        print(template);
        return;
    }
    int total_sec, hh, mm, ss, cs;
    lastTick = tickss = ticks();
    long total_ms = (tickss - startTicks + (accumTicks))*55;
    total_sec = total_ms/1000;
    cs = (total_ms % 1000)/100;

    hh = total_sec/3600;
	mm = (total_sec - hh*3600)/60;
	ss = total_sec - hh*3600 - mm*60;

    twoDigitsNumToStr(hh,&template[0]);
    twoDigitsNumToStr(mm,&template[3]);
    twoDigitsNumToStr(ss,&template[6]);
    twoDigitsNumToStr(cs,&template[9]);
    printColor(template,RED);

}
