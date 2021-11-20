

headers += $(shell find $(board_dir) -name '*.h')
c_src += $(shell find $(board_dir) -name '*.c')
S_src += $(shell find $(board_dir) -name '*.S')
lds := $(board_dir)/kernel.lds

AS = arm-none-eabi-gcc
AS_FLAGS = -I$(board_dir) -c -Wextra -ffreestanding -mcpu=arm920t

CC_FLAGS += -mcpu=arm920t
#AS_FLAGS += -mcpu=arm920t

