

headers += $(shell find $(board_dir) -name '*.h')
hppeaders += $(shell find $(board_dir) -name '*.hpp')
c_src += $(shell find $(board_dir) -name '*.c')
S_src += $(shell find $(board_dir) -name '*.S')
cpp_src += $(shell find $(board_dir) -name '*.cpp')
lds := $(board_dir)/kernel.lds

AS = arm-none-eabi-gcc
#AS_FLAGS += -c -Wextra -ffreestanding -mcpu=arm920t -D__ASSEMBLER__

AS_FLAGS += -c -Wextra -ffreestanding -march=armv4t -D__ASSEMBLER__

#CC_FLAGS += -mcpu=arm920t
CC_FLAGS += -march=armv4t
CXX_FLAGS += -march=armv4t
#LD_PRE_FLAGS += -L /usr/lib/gcc/arm-none-eabi/9.2.1/ -L /usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib/arm-none-eabi/9.2.1/ -L/usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib/
#LD_POST_FLAGS += -static -lstdc++ -lc -lgcc
#AS_FLAGS += -mcpu=arm920t

