include Makefile.inc
KERNEL=kernel.bin

SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_INT=$(wildcard interrupts/*.c)
SOURCES_DRIVER=$(wildcard drivers/*.c)


OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_INT=$(SOURCES_INT:.c=.o)
OBJECTS_DRIVER=$(SOURCES_DRIVER:.c=.o)

LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)

MEM_MANAGER = MEM_MANAGER_PROPIO

STATICLIBS=

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_INT) $(OBJECTS_DRIVER)
	@$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_INT) $(OBJECTS_DRIVER) $(STATICLIBS)

	@$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_INT) $(OBJECTS_DRIVER) $(STATICLIBS)

%.o: %.c
	@$(GCC) $(GCCFLAGS) -D $(MEM_MANAGER) -I./include -I./interrupts -I./drivers -c $< -o $@ 

%.o : %.asm
	@$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	@$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf drivers/*.o interrupts/*.o asm/*.o *.o *.bin kernel.elf

.PHONY: all clean
