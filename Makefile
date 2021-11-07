ARMGNU ?= arm-none-eabi
#ARMGNU ?= arm-linux-gnueabi

CC = $(ARMGNU)-gcc
AS = $(ARMGNU)-as

AS_FLAGS = --warn --fatal-warnings
CC_FLAGS = -Wall -Werror -nostdlib -nostartfiles -ffreestanding -c

headers = $(wildcard *.h) $(wildcard */*.h)
c_src = $(wildcard *.c) $(wildcard */*.c)
S_src = $(wildcard *.S) $(wildcard */*.S)

obj = $(c_src:.c=.o) $(S_src:.S=.o)
lds = kernel.lds

all : kernel.img

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.hex
	rm -f *.srec
	rm -f *.elf
	rm -f *.list
	rm -f *.img

#vectors.o : vectors.s
#	$(ARMGNU)-as $(AS_FLAGS) vectors.s -o vectors.o
#
#notmain.o : notmain.c
#	$(ARMGNU)-gcc $(CC_FLAGS) -c notmain.c -o notmain.o

%.o: %.S
	$(AS) $(AS_FLAGS) -o $@ $<

%.o: %.c $(headers)
	$(CC) $(CC_FLAGS) -o $@ $<

kernel.elf : $(obj) $(lds)
	$(ARMGNU)-ld -T $(lds) -o $@ $(obj)
#	$(ARMGNU)-objdump -D notmain.elf > notmain.list

kernel.img : kernel.elf
	$(ARMGNU)-objcopy --srec-forceS3 kernel.elf -O srec kernel.srec
	$(ARMGNU)-objcopy kernel.elf -O binary kernel.img






#headers = $(wildcard *.h) $(wildcard */*.h)
#c_src = $(wildcard *.c) $(wildcard */*.c)
#S_src = $(wildcard *.S) $(wildcard */*.S)
#obj = $(c_src:.c=.o) $(S_src:.S=.o)
#prod = kernel.elf
#lds = kernel.lds
#dumps = $(obj:.o=.list) $(prod:.elf=.list)
#
#AS = arm-none-eabi-as
#CC = arm-none-eabi-gcc
#OC = arm-none-eabi-objcopy
#CC_FLAGS = -Wall -Werror -nostdlib -nostartfiles -ffreestanding
#CC_DEBUG_FLAGS = -g
#LD = arm-none-eabi-ld
#LD_FLAGS = 
#DUMP = arm-none-eabi-objdump
#DUMP_FLAGS = -fhd
#QEMU = links/qemu
#QEMU_FLAGS = -M portux920t -m 64M -nographic
#QEMU_DEBUG_FLAGS = -s -S
#
#all: $(prod) $(dumps) kernel.img
#	@echo as: $(S_src)
#	@echo c: $(c_src)
#	@echo headers: $(headers)
#	@echo obj: $(obj)
#	@echo prod: $(prod)
#	@echo dumps: $(dumps)
#
#%.o: %.S
#	$(AS) $(AS_FLAGS) -o $@ $<
#
#%.o: %.c $(headers)
#	$(CC) -o $@ $(CC_FLAGS) $<
#
#%.elf: $(obj) $(lds)
#	$(LD) $(LD_FLAGS) -T $(lds) -o $@ $(obj)
#
#%.list: %.o
#	$(DUMP) $< $(DUMP_FLAGS) > $@
#
#%.list: %.elf
#	$(DUMP) $< $(DUMP_FLAGS) > $@
#
#kernel.img: $(prod)
#	$(OC) --srec-forceS3 $(prod) -O srec kernel.srec
#	$(OC) $(prod) -O binary kernel.img
#
#run: $(prod)
#	$(QEMU) $(QEMU_FLAGS) -kernel $(prod)
#
#debug: CC_FLAGS += $(CC_DEBUG_FLAGS)
#debug: QEMU_FLAGS += $(QEMU_DEBUG_FLAGS)
#debug: $(prod) $(dumps)
#	@echo run gdb-multiarch -s kernel.elf -ex "\"target remote localhost:1234\"" -ex "\"layout split\""
#	$(QEMU) $(QEMU_FLAGS) -kernel $(prod)
#
#wordcount:
#	cat $(S_src) $(c_src) $(headers) | wc
#
#.SECONDARY: $(obj)
#.PHONY: clean
#clean:
#	rm -f $(obj) $(prod) $(dumps)
#
#dependencies:
#	sudo apt install libglib2.0-dev libgcrypt20-dev zlib1g-dev autoconf automake libtool bison flex libpixman-1-dev
#	sudo apt install gdb-multiarch gcc-arm-none-eabi
#