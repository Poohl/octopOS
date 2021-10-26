headers = $(wildcard *.h)
c_src = $(wildcard *.c)
S_src = $(wildcard *.S)
obj = $(c_src:.c=.o) $(S_src:.S=.o)
prod = kernel.elf
lds = kernel.lds
dumps = $(obj:.o=.list) $(prod:.elf=.list)

AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
CC_FLAGS = -Wall -mcpu=arm920t -O2 -c
CC_FLAGS += -Wextra -ffreestanding
LD = arm-none-eabi-ld
LD_FLAGS = 
DUMP = arm-none-eabi-objdump
DUMP_FLAGS = -fhd
QEMU_FLAGS = -M portux920t -m 64M -nographic
QEMU_DEBUG_FLAGS = -s -S

all: $(prod) $(dumps)
	@echo as: $(S_src)
	@echo c: $(c_src)
	@echo obj: $(obj)
	@echo prod: $(prod)
	@echo dumps: $(dumps)

%.o: %.S
	$(AS) $(AS_FLAGS) -o $@ $<

%.o: %.c $(headers)
	$(CC) -o $@ $(CC_FLAGS) $<

%.elf: $(obj) $(lds)
	$(LD) $(LD_FLAGS) -T$(lds) -o $@ $(obj)

%.list: %.o
	$(DUMP) $< $(DUMP_FLAGS) > $@

%.list: %.elf
	$(DUMP) $< $(DUMP_FLAGS) > $@

run: $(prod)
	$(QEMU) $(QEMU_FLAGS) $(prod)

debug: $(prod) $(dumps)
	@echo run gdb-multiarch -ex "\"target remote localhost:1234\""
	$(QEMU) $(QEMU_FLAGS) $(QEMU_DEBUG_FLAGS) -kernel $(prod)

.SECONDARY: $(obj)
.PHONY: clean
clean:
	rm -f $(obj) $(prod) $(dumps)

dependencies:
	sudo apt install libglib2.0-dev libgcrypt20-dev zlib1g-dev autoconf automake libtool bison flex libpixman-1-dev
	sudo apt install gdb-multiarch gcc-arm-none-eabi
