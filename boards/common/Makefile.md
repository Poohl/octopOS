# How to create the plugin Makefile

## How to read this Guide

everything in multiline code samples should go into the actual file called `Makefile` (case sensitive) inside your board directory.

## How the Makefile is loaded

Your board-makefile is included after all input parameters are set but before the output parameters are loaded and the targets defined (exception is the `all` target, it's defined before but loaded after)

This has a few effects to keep in mind:
* You have full control over used programs, sources and flags
* you should (unless you know what you're doing) only ever append to the lists. Otherwise you can e.g. delete the flag that tells the compiler to compile...
* none of your extra products need to be prefixed with `$(build_dir)`, this is done automatically. just put them in `prod`
* however, this can also be a problem
* as the file is included not executed, you need to prefix everything with `$(board_dir)` which is a variable loaded with the path to your board's directory.

# THE PLUGIN PART

Technically it doesn't matter in what order you put your shit in the file, however by convenstion there's a part that deals with "required" setup before the one for everything else.

## appending to the list of sources
You need to insert everyhting you want to build into the respective list. The given example assumes you want everything in your directory build (unused symbols are thrown away).
```
headers := $(shell find $(board_dir) -name '*.h')
c_src := $(shell find $(board_dir) -name '*.c')
S_src := $(shell find $(board_dir) -name '*.S')
```

## linker file to use
Rather self explanatory, the linker file to use for linking
```
lds := $(board_dir)/kernel.lds
```

# The "the defaults won't do" part

## Your modifications to the build parameters
Most commonly you need to set your architecute, your subfolders as included or the location of gcclib. Only ever APPEND don't set them, as again, you would overwrite the default flags.
```
AS_FLAGS += -march=armv6-m
CC_FLAGS += -march=armv6-m -I$(board_dir)/headers
LD_POST_FLAGS += /usr/lib/gcc/arm-none-eabi/9.2.1/thumb/v6-m/nofp/libgcc.a
```

## any extras you need to have built
They go into the `prod` variable. By default only `kernel.elf` is in there.
```
prod += kernel.bin
```
## and how to build them

```
%.bin: %.elf
	$(ARMGNU)-objcopy --srec-forceS3 $^ -O srec $(build_dir)/kernel.srec
	$(ARMGNU)-objcopy $(build_dir)/kernel.elf -O binary $@
```
note that when you want to access source files directly (e.g. `kernel.lds`) they don't have the `$(build_dir)` prefix, so the good `%.out: %.in` wont't work. Either use the full path in the rule like so:
```
$(build_dir)/foo/kernel.img: foo/kernel.c
```
or use a static rule, that explicitly defines what targets can be built using it:
```
$(build_dir)/foo/kernel.bin $(build_dir)/bar/kernel.bin: $(build_dir)/%.bin: %.c
```

## any other extra targets you might want
usually one would expect at least a `run` and a `debugger`.
```
remote_debugger: $(prod)
	scp $(build_dir)/kernel.elf $(target):/tmp/
	ssh $(target) -c gdb /tmp/kernel.elf
```
The names `all` and `_all` are reserved

# Insane extra stuff

You could have more targets, more varibles, whatever you want...

You might have noticed that the paramters passed to `make` are also just variables one could use for even more fancy behaviour.

You can also put entire sections of other makefiles here, only thing to keep in mind are the `$(build_dir)` and `$(board_dir)` variables.