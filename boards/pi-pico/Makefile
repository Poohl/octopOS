

headers += $(shell find $(board_dir) -name '*.h')
c_src += $(shell find $(board_dir) -name '*.c')
S_src += $(shell find $(board_dir) -name '*.S')
cpp_src += $(shell find $(board_dir) -name '*.cpp')
lds := $(board_dir)/kernel.lds

#prod += kernel.uf2
%.uf2: %.elf
	links/elf2uf2 $^ $@

#LD_FLAGS += $(board_dir)/pi-pico.a
#LD_PRE_FLAGS += -march=armv6-m
LD_POST_FLAGS += /usr/lib/gcc/arm-none-eabi/9.2.1/thumb/v6-m/nofp/libgcc.a

AS = arm-none-eabi-gcc
#LD = gold
CC_FLAGS += -march=armv6s-m -I./$(board_dir)/drivers -std=gnu11 -Wno-override-init
CXX_FLAGS += -march=armv6s-m -I./$(board_dir)/drivers -std=gnu11 -Wno-override-init
#CXX_FLAGS += -I./$(board_dir)/drivers -std=gnu11 -Wno-override-init
AS_FLAGS += -c -Wextra -ffreestanding -D__ASSEMBLER__ -march=armv6s-m
DUMP_FLAGS += -M force-thumb -z -m armv6s-m
