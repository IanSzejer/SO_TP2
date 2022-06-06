#include "keyboard.h"


static int scancodeToAscii[255] = {
0, // 0
ESC, // 1
'1', // 2
'2', // 3
'3', // 4
'4', // 5
'5', // 6
'6', // 7
'7', // 8
'8', // 9
'9', // 10
'0', // 11
'-', // 12
'=', // 13
'\b', // 14
'\t', // 15
'q', // 16
'w', // 17
'e', // 18
'r', // 19
't', // 20
'y', // 21
'u', // 22
'i', // 23
'o', // 24
'p', // 25
'[', // 26
']', // 27
'\n', // 28
0, // 29
'a', // 30
's', // 31
'd', // 32
'f', // 33
'g', // 34
'h', // 35
'j', // 36
'k', // 37
'l', // 38
';', // 39
'\'', // 40
'`', // 41
0, // 42
'\\', // 43
'z', // 44
'x', // 45
'c', // 46
'v', // 47
'b', // 48
'n', // 49
'm', // 50
',', // 51
'.', // 52
'/', // 53
0, // 54
0, // 55
ALT, // 56
' ', // 57
CAPS_LOCK, // 58
0, // 59
0, // 60
0, // 61
0, // 62
0, // 63
0, // 64
0, // 65
0, // 66
0, // 67
0, // 68
0, // 69
0, // 70
0, // 71
UP_ARROW,  // 72
0, // 73
0, // 74
LEFT_ARROW,  // 75 
0, // 76
RIGHT_ARROW, // 77
0, // 78
0, // 79
DOWN_ARROW, // 80
};

static int MAYUS = 0;
static int * target = 0;
static int pressedValue = 0;
uint64_t stackBackup[16]= {0};


int getAscii(int val) {
     MAYUS += (val == CAPS_LOCK) ? ( MAYUS ? -1 : 1) : 0;

	int ascii = scancodeToAscii[val];

	return ascii == 0 ? '?' : ( IS_ALPHA(ascii) ? ascii - MAYUS_DIFF*MAYUS : ascii);
}


int kb_read() {
	int sc;
     sc = kbReadUntilCode();

	return getAscii(sc);
}

void overwriteRegs() {
     updateRegs(stackBackup);
}


void keyboardDriver(uint64_t * stack) {
     for (int i=0 ; i<REG_COUNT-1 ; i++)
          stackBackup[i] = stack[i];
     int c;
     while(keyboardActivated()) {
          c = getKbCode();
     }
     if (target && !(c & 0x80)) {
          *target = getAscii(c);
          if (*target == ALT)
               overwriteRegs();
     }
          

}
      	

void set_kb_target(int * var) {
     target = var;
}

int readFromKeyboard(char * buf, uint64_t count, int ascii) {
     //picMasterMask(0xFE);
     for(int i=0 ; i<count ; i++) {
          buf[i] = kb_read();
          pressedValue = buf[i];
          if (ascii && !PRINTABLE(buf[i]))
               i--;
     }
     return count;
}