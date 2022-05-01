#include <lib.h>



extern uint64_t* inforeg();

static char registers[REG_COUNT][20] = {
	"R15 = 0x", "R14 = 0x", "R13 = 0x", "R12 = 0x", "R11 = 0x", "R10 = 0x", "R9 = 0x",
	"R8 = 0x", "RSI = 0x", "RDI = 0x", "RBP = 0x", "RDX = 0x", "RCX = 0x", "RBX = 0x",
	"RAX = 0x","RIP = 0x","RSP = 0x"};

void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	*p = 0;

	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

static void fillCommonRegs(uint64_t* regs) {
     for (int i=0 ; i<REG_COUNT-2 ; i++) {
        char * ptr = registers[i][4] == '=' ? &registers[i][8] : &registers[i][7];
        int len = uintToBase(regs[i], ptr,16);
        ptr[len] = 0;
    }
}

void updateRegs(uint64_t* regs) {

   fillCommonRegs(regs);

    char * sp = &registers[REG_COUNT-1][8];
    uint64_t * stack = (uint64_t *)regs[15]; 

    int len = (int)uintToBase((uint64_t)(stack), sp, 16); 
    sp[len] = 0;
    
    sp = &registers[REG_COUNT-2][8];
    len = (int)uintToBase((uint64_t)(*stack), sp, 16);
    sp[len] = 0;
        
}


void getRegs(char ** buf) {
        for (int i=0 ; i<REG_COUNT ; i++) {
            buf[i] = registers[i];
        }
}


