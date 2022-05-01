include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_APPS=$(wildcard apps/*.c)
SOURCES_SCREENMANAGEMENT=$(wildcard screenManagement/*.c)



OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_APPS=$(SOURCES_APPS:.c=.o)
OBJECTS_SCREENMANAGEMENT=$(SOURCES_SCREENMANAGEMENT:.c=.o)

all: $(MODULE)

$(MODULE): $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_APPS) $(OBJECTS_SCREENMANAGEMENT)
	@$(LD) $(LDFLAGS) -T sampleCodeModule.ld _loader.o $(filter-out _loader.o,$(OBJECTS)) $(OBJECTS_ASM) $(OBJECTS_APPS) $(OBJECTS_SCREENMANAGEMENT) -o ../$(MODULE)

	@$(LD) $(LDFLAGS) -I./include -T sampleCodeModule.ld _loader.o $(filter-out _loader.o,$(OBJECTS)) $(OBJECTS_ASM) $(OBJECTS_APPS) $(OBJECTS_SCREENMANAGEMENT) --oformat=elf64-x86-64 -o ../0000-sampleCodeModule.elf

%.o : %.asm
	@$(ASM) $(ASMFLAGS) $< -o $@

%.o: %.c
	@$(GCC) $(GCCFLAGS) -I./include -I./apps -I./screenManagement -c $< -o $@

clean:
	rm -rf *.o apps/*.o screenManagement/*.o asm/*.o *.bin 0000-sampleCodeModule.elf

.PHONY: all clean print