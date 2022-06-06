#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <videoD.h>
#include <idtLoader.h>
#include <keyboard.h>
#include <clock.h>
#include <scheduler.h>
#include <interrupts.h>
#include <exceptions.h>
#include <memoryDriver.h>
extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void * const heapModuleAdress =(void*) 0x600000;
static void * const maxSize =(void*) 0x10000;
typedef int (*EntryPoint)();



void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{

	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	

	clearBSS(&bss, &endOfKernel - &bss);


	return getStackBase();
}




int main()
{		
	//setExceptionRebootPoint((uint64_t)sampleCodeModuleAddress, getStack());
	
	initMemManager(heapModuleAdress,maxSize);
	ncClear();
	
	initializeScheduler(sampleCodeModuleAddress);	
	//((EntryPoint)sampleCodeModuleAddress)();
	load_idt();
	while(1){
		
	}
	return 0;
}
