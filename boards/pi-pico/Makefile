

headers := $(shell find $(board_dir) -name '*.h')
c_src := $(shell find $(board_dir) -name '*.c')
S_src := $(shell find $(board_dir) -name '*.S')
lds := $(board_dir)/kernel.lds

#LD_FLAGS += $(board_dir)/pi-pico.a
LD_POST_FLAGS += /usr/lib/gcc/arm-none-eabi/9.2.1/thumb/v6-m/nofp/libgcc.a

CC_FLAGS += -march=armv6-m -mthumb -I./$(board_dir)/drivers -std=gnu11
AS_FLAGS += -march=armv6-m -mthumb
DUMP_FLAGS += -M force-thumb -z
