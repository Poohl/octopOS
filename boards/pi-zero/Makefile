## THE PLUGIN PART

# this assumes you want everything in your directory build
headers += $(shell find $(board_dir) -name '*.h')
c_src += $(shell find $(board_dir) -name '*.c')
S_src += $(shell find $(board_dir) -name '*.S')

# linker file to use
lds := $(board_dir)/kernel.lds

## END OF PLUGIN PART
## Everything below is optional, as there are sensible defaults


# any extras you neet to have built
prod += kernel.bin

# and how to build them (could use full name here aswell)
%.bin: %.elf
	$(ARMGNU)-objcopy --srec-forceS3 $^ -O srec $(build_dir)/kernel.srec
	$(ARMGNU)-objcopy $(build_dir)/kernel.elf -O binary $@


ARMGNU ?= arm-none-eabi
#ARMGNU ?= arm-linux-gnueabi

CC = $(ARMGNU)-gcc
AS = $(ARMGNU)-as

AS_FLAGS = --warn --fatal-warnings
CC_FLAGS += -nostdlib -nostartfiles

#CC_FLAGS += -Werror

#vectors.o : vectors.s
#	$(ARMGNU)-as $(AS_FLAGS) vectors.s -o vectors.o
#
#notmain.o : notmain.c
#	$(ARMGNU)-gcc $(CC_FLAGS) -c notmain.c -o notmain.o
