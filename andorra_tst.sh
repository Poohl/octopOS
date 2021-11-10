# test the kernel on andorra using Leo's build of qemu
# run: $ sh andorra_tst.sh [FU_USERNAME]

# copy kernel to $HOME on andorra
scp ./release-portux/kernel.elf $1@andorra.imp.fu-berlin.de:~/tmp_kernel.elf
# run kernel then remove it
ssh $1@andorra.imp.fu-berlin.de \
'/home/mi/koenigl/Git/qemu-build/arm-softmmu/qemu-system-arm \
 -nographic -M portux920t -m 64M -kernel tmp_kernel.elf \
 ; rm tmp_kernel.elf'
