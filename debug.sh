gdb-multiarch -s kernel.elf -ex \
"target remote localhost:1234" -ex "layout split"
