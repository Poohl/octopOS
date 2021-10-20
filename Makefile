
dependencies:
	sudo apt install libglib2.0-dev libgcrypt20-dev zlib1g-dev autoconf automake libtool bison flex libpixman-1-dev
	sudo apt install gdb-multiarch arm-none-eabi-gcc

headers = $(wildcard *.h)
src = $(wildcard *.c)
obj = $(src:.c=.o)
prod = kernel.elf

CC = arm-none-eabi-gcc
CC_FLAGS = -march=armv4 -Wall -mcpu=arm920t -O2 -c
CC_FLAGS += -Wextra -ffreestanding
LD = arm-none-eabi-ld
LD_FLAGS = 
QEMU = qemu
QEMU_FLAGS = M portux920t -m 64M -nographic -kernel a.out
QEMU_DEBUG_FLAGS = -s -S

test:
	echo $(src)

all: $(prod)

%.o: %.c $(headers)
	$(CC) -o $@ $(CC_FLAGS) $<

%.elf: $(obj)
	$(LD) $(LD_FLAGS) -o $@ $<

run: $(prod)
	$(QEMU) $(QEMU_FLAGS) -kernel $(prod)

debug: $(prod)
	echo run gdb-multiarch target remote localhost:1234
	$(QEMU) $(QEMU_FLAGS) $(QEMU_DEBUG_FLAGS) -kernel $(prod)

.SECONDARY: $(obj)
.PHONY: clean
clean:
	rm -f $(obj) $(prod)
