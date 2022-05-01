
#include <exceptions.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OP_EXCEPTION_ID 6
#define RED_COLOR 4

// static char registers[16][20] = {
// 	"RAX: 0x", "RBS: 0x", "RCX: 0x", "RDX: 0x", "RBP: 0x", "RDI: 0x", "RSI: 0x",
// 	"R8: 0x", "R9: 0x", "R10: 0x", "R11: 0x", "R12: 0x", "R13: 0x", "R14: 0x",
// 	"R15: 0x"};

// static uint64_t * regInfoPTR;

static void zero_division();
static void invalid_op();
extern void infoReg(char ** buf);

static uint64_t restoreIp;;
static uint64_t * restoreSp;

void setExceptionRebootPoint(uint64_t ip, uint64_t * sp) {
	restoreIp = ip;
	restoreSp = sp;
}


void exceptionDispatcher(int exception, uint64_t * regs) {
	switch(exception) {
		case ZERO_EXCEPTION_ID:
		zero_division();
		break;
		case INVALID_OP_EXCEPTION_ID:
		invalid_op();
		break;
	}
	int s = seconds();

	char * buf[17];
	updateRegs(regs);
	getRegs(buf);
	
	for (int i=0 ; i<17 ; i++) {
		ncPrint(buf[i]);
		ncNewline();
	}

	while(seconds() - s < 3 && seconds() >= s);

	uint64_t * prevStack = (uint64_t *)regs[15]; // pisamos el valor del stack
	regs[15] = (uint64_t )restoreSp;
	restoreSp[0] = restoreIp;
	restoreSp[1] = prevStack[1];
	restoreSp[2] = prevStack[2];
}

static void zero_division() {
	// Handler para manejar excepcíon
	ncSplitConsole(1,0);
	ncClear();
	ncPrintColor("Exception: division by zero",RED_COLOR);
	ncNewline();
}

static void invalid_op() {
	// Handler para manejar excepcíon
	ncSplitConsole(1,0);
	ncClear();
	ncPrintColor("Exception: invalid operation code",RED_COLOR);
	ncNewline();
}