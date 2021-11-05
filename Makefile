# what platform to build for
platform:=pi-pico
# what folder to build into
# cannot include ../
build=release-$(platform)

AS = arm-none-eabi-as
AS_FLAGS =
AS_DEBUG_FLAGS = -DDEBUG

CC = arm-none-eabi-gcc
CC_FLAGS = -Wall -O2 -c -Wextra -ffreestanding
CC_FLAGS += -I. -I./boards/$(platform) -include default.h
CC_DEBUG_FLAGS = -g -DDEBUG

LD = arm-none-eabi-ld
LD_FLAGS = 
LD_DEBUG_FLAGS = 

DUMP = arm-none-eabi-objdump
DUMP_FLAGS = -fhd
DUMP_DEBUG_FLAGS = 

QEMU = links/qemu
QEMU_FLAGS = -M portux920t -m 64M -nographic
QEMU_DEBUG_FLAGS = -s -S

src_dirs = apps drivers libs fluff

# load board config
board_dir := boards/$(platform)

include $(board_dir)/Makefile

# fix path
#headers := $(addprefix boards/$(platform),$(headers))
#c_src := $(addprefix boards/$(platform),$(c_src))
#S_src := $(addprefix boards/$(platform),$(S_src))
#lds := $(addprefix boards/$(platform),$(lds))

# load own sources
headers += $(shell find $(src_dirs) -name '*.h')
c_src += $(shell find $(src_dirs) -name '*.c')
S_src += $(shell find $(src_dirs) -name '*.S')

# load list of things to produce
obj := $(c_src:.c=.o) $(S_src:.S=.o)
prod := kernel.elf
dumps := $(obj:.o=.list) $(prod:.elf=.list)

obj := $(addprefix $(build)/,$(obj))
prod := $(addprefix $(build)/,$(prod))
dumps := $(addprefix $(build)/,$(dumps))

# workaround for pattern matching ideosyncracy
c_obj:=$(shell find . -name '*.c')
S_obj:=$(shell find . -name '*.S')
c_obj:= $(c_obj:.c=.o)
S_obj:= $(S_obj:.S=.o)
c_obj:= $(addprefix $(build)/,$(c_obj))
S_obj:= $(addprefix $(build)/,$(S_obj))
# 50% of the guys writing this shite were actually monkeys
# what would you expect path-truncate to do for nonexistant files?
#c_obj:= $(realpath $(c_obj))
#S_obj:= $(realpath $(S_obj))
c_obj:= $(subst ./,,$(c_obj))
S_obj:= $(subst ./,,$(S_obj))



all: $(prod) $(dumps)
	@echo as: $(S_src)
	@echo c: $(c_src)
	@echo headers: $(headers)
	@echo obj: $(obj)
	@echo prod: $(prod)
	@echo dumps: $(dumps)

$(S_obj): $(build)/%.o: %.S
	@mkdir -p $(@D)
	$(AS) $(AS_FLAGS) -o $@ $<

$(c_obj): $(build)/%.o: %.c $(headers)
	@mkdir -p $(@D)
	$(CC) -o $@ $(CC_FLAGS) $<

$(prod): $(build)/%.elf: $(obj) $(lds)
	@mkdir -p $(@D)
	$(LD) $(LD_FLAGS) -T $(lds) -o $@ $(obj)

%.list: %.o
	@mkdir -p $(@D)
	$(DUMP) $< $(DUMP_FLAGS) > $@

%.list: %.elf
	@mkdir -p $(@D)
	$(DUMP) $< $(DUMP_FLAGS) > $@

run: $(prod)
	$(QEMU) $(QEMU_FLAGS) -kernel $(prod)

debug: AS_FLAGS += $(AS_DEBUG_FLAGS)
debug: CC_FLAGS += $(CC_DEBUG_FLAGS)
debug: LD_FLAGS += $(LD_DEBUG_FLAGS)
debug: QEMU_FLAGS += $(QEMU_DEBUG_FLAGS)
debug: build = debug-$(platform)
debug: $(prod) $(dumps)
	@echo Debug build complete

debug_run: debug
	@echo run gdb-multiarch -s $(prod) -ex "\"target remote localhost:1234\"" -ex "\"layout split\""
	$(QEMU) $(QEMU_FLAGS) -kernel $(prod)

debugger:
	run gdb-multiarch -s $(prod) -ex "\"target remote localhost:1234\"" -ex "\"layout split\""

wordcount:
	cat $(S_src) $(c_src) $(headers) | wc

clean:
	rm -rf $(build)

dependencies:
	sudo apt install libglib2.0-dev libgcrypt20-dev zlib1g-dev autoconf automake libtool bison flex libpixman-1-dev
	sudo apt install gdb-multiarch gcc-arm-none-eabi

make_var_test:
	@echo c_obj: $(c_obj)
	@echo S_obj: $(S_obj)
	@echo build: $(build)
	@echo CC_FLAGS: $(CC_FLAGS)
	@echo AS_FLAGS: $(AS_FLAGS)
	@echo QEMU_FLAGS: $(QEMU_FLAGS)
	@echo as: $(S_src)
	@echo c: $(c_src)
	@echo headers: $(headers)
	@echo obj: $(obj)
	@echo prod: $(prod)
	@echo dumps: $(dumps)

.SECONDARY: $(obj)
.PHONY: clean

.SUFFIXES: