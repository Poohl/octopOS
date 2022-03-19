# what platform to build for
platform:=portux
# what version to build, available:
# - release
# - debug
build:=release

# debugger target:
debug_target=localhost:1234
# if the debugger should load the binary first
debug_load=

# where to build to, the used value is just a default
build_dir:=$(build)-$(platform)

all: _all

# used software and default flags
AS = arm-none-eabi-as
AS_FLAGS = $(addprefix -I,$(INCLUDE))
AS_DEBUG_FLAGS =

INCLUDE = . ./boards/$(platform)

CC = arm-none-eabi-gcc
CC_FLAGS = -Wall -O2 -c -Wextra -ffreestanding -Wno-override-init -std=gnu17
CC_FLAGS += $(addprefix -I,$(INCLUDE)) -include default.h
CC_DEBUG_FLAGS = -g -DDEBUG

CXX = arm-none-eabi-g++
CXX_FLAGS = -Wall -O2 -c -Wextra -ffreestanding -Werror=return-type -Werror=misleading-indentation -fno-rtti -nostartfiles
CXX_FLAGS += $(addprefix -I,$(INCLUDE)) -include default.hpp
CXX_DEBUG_FLAGS = -g -DDEBUG

LD = arm-none-eabi-ld
LD_PRE_FLAGS =
LD_DEBUG_PRE_FLAGS = 
LD_POST_FLAGS = 
LD_DEBUG_POST_FLAGS = 

DUMP = arm-none-eabi-objdump
DUMP_FLAGS = -fhd
DUMP_DEBUG_FLAGS = 

QEMU = links/qemu
QEMU_FLAGS = -M portux920t -m 64M -nographic
QEMU_DEBUG_FLAGS = -s -S

# folders that include general code used regardless of platform
#src_dirs = apps drivers libs fluff kernel
src_dirs = drivers libs fluff kernel

# fix path
#headers := $(addprefix boards/$(platform),$(headers))
#c_src := $(addprefix boards/$(platform),$(c_src))
#S_src := $(addprefix boards/$(platform),$(S_src))
#lds := $(addprefix boards/$(platform),$(lds))

# load general sources
headers += $(shell find $(src_dirs) -name '*.h' -name '*.hpp')
#hppeaders += $(shell find $(src_dirs) -name '*.h')
c_src += $(shell find $(src_dirs) -name '*.c')
cpp_src += $(shell find $(src_dirs) -name '*.cpp')
S_src += $(shell find $(src_dirs) -name '*.S')

# load platform specific config and sources
board_dir := boards/$(platform)
include $(board_dir)/Makefile

# load list of things to produce
obj += $(c_src:.c=.o) $(S_src:.S=.o) $(cpp_src:.cpp=.o)
prod += kernel.elf
dumps += $(obj:.o=.list) $(prod:.elf=.list)

# fix path of produeced outputs
obj := $(addprefix $(build_dir)/,$(obj))
prod := $(addprefix $(build_dir)/,$(prod))
#dumps := $(addprefix $(build_dir)/,$(dumps))

# workaround for pattern matching ideosyncracy
c_obj:=$(shell find . -name '*.c')
S_obj:=$(shell find . -name '*.S')
cpp_obj:=$(shell find . -name '*.cpp')
c_obj:= $(c_obj:.c=.o)
S_obj:= $(S_obj:.S=.o)
cpp_obj:= $(cpp_obj:.cpp=.o)
c_obj:= $(addprefix $(build_dir)/,$(c_obj))
S_obj:= $(addprefix $(build_dir)/,$(S_obj))
cpp_obj:= $(addprefix $(build_dir)/,$(cpp_obj))
# 50% of the guys writing this shite were actually monkeys
# what would you expect path-truncate to do for nonexistant files?
#c_obj:= $(realpath $(c_obj))
#S_obj:= $(realpath $(S_obj))

c_obj:= $(subst ./,,$(c_obj))
S_obj:= $(subst ./,,$(S_obj))
cpp_obj:= $(subst ./,,$(cpp_obj))

ifeq ($(build), debug)
AS_FLAGS += $(AS_DEBUG_FLAGS)
CC_FLAGS += $(CC_DEBUG_FLAGS)
LD_PRE_FLAGS += $(LD_DEBUG_PRE_FLAGS)
LD_POST_FLAGS += $(LD_DEBUG_POST_FLAGS)
QEMU_FLAGS += $(QEMU_DEBUG_FLAGS)
CXX_FLAGS += $(CXX_DEBUG_FLAGS)
endif

_all: $(prod) $(dumps)
	@echo as: $(S_src)
	@echo c: $(c_src)
	@echo cpp: $(cpp_src)
	@echo headers: $(headers)
	@echo obj: $(obj)
	@echo prod: $(prod)
	@echo dumps: $(dumps)

$(S_obj): $(build_dir)/%.o: %.S
	@mkdir -p $(@D)
	$(AS) $(AS_FLAGS) -o $@ $<

$(c_obj): $(build_dir)/%.o: %.c $(headers)
	@mkdir -p $(@D)
	$(CC) -o $@ $(CC_FLAGS) $<

$(cpp_obj): $(build_dir)/%.o: %.cpp $(headers)
	@mkdir -p $(@D)
	$(CXX) -o $@ $(CXX_FLAGS) $<

# this assumes the output is always called kernel.elf
$(build_dir)/kernel.elf: $(obj) $(lds)
	@mkdir -p $(@D)
	$(LD) $(LD_PRE_FLAGS) -T $(lds) -o $@ $(obj) $(LD_POST_FLAGS)

%.list: %.o
	@mkdir -p $(@D)
	$(DUMP) $< $(DUMP_FLAGS) > $@

%.list: %.elf
	@mkdir -p $(@D)
	$(DUMP) $< $(DUMP_FLAGS) > $@

run: $(prod)
	$(QEMU) $(QEMU_FLAGS) -kernel $(prod)

dumps: $(dumps)

debugger: $(prod) $(dumps)
	gdb-multiarch $(if $(debug_load),,-s) $(build_dir)/kernel.elf -ex "target remote $(debug_target)" -ex "layout split" $(if $(debug_load), --ex "monitor reset init" --ex "monitor reset init" --ex "load")

wordcount:
	cat $(S_src) $(c_src) $(headers) | wc

clean:
	rm -rf $(build_dir)

dependencies:
	sudo apt install libglib2.0-dev libgcrypt20-dev zlib1g-dev autoconf automake libtool bison flex libpixman-1-dev
	sudo apt install gdb-multiarch gcc-arm-none-eabi

make_var_test:
	@echo c_obj: $(c_obj)
	@echo S_obj: $(S_obj)
	@echo cpp_obj: $(cpp_obj)
	@echo build_dir: $(build_dir)
	@echo CC_FLAGS: $(CC_FLAGS)
	@echo AS_FLAGS: $(AS_FLAGS)
	@echo CXX_FLAGS: $(CXX_FLAGS)
	@echo QEMU_FLAGS: $(QEMU_FLAGS)
	@echo as: $(S_src)
	@echo c: $(c_src)
	@echo cpp: $(cpp_src)
	@echo headers: $(headers)
	@echo obj: $(obj)
	@echo prod: $(prod)
	@echo dumps: $(dumps)

.SECONDARY: $(obj)
.PHONY: clean

.SUFFIXES:
