# octopOS for the RasPiZero

Your favorite OS is now available for the RaspberryPi Zero. Well, printf works so far...

## getting started

1. get `bootcode.bin` and `start.elf` from https://github.com/raspberrypi/firmware/tree/master/boot
2. put both on an empty sd-card (FAT32)
3. build the project:
    ```
    $ make
    ```
4. there are two ways to get octopOS on your raspi...
    - 4.a 
        * copy `kernel.img` to the sd-card and put it into the raspi. done. 
        * (there is no way of knowing what is going on though. what you really want is a fancy uart-connection...)
    - 4.b.
        * get the super cool bootloader from https://github.com/dwelch67/raspberrypi-zero. either get `bootloader.img` or build it    yourself (from `/bootloader10`). rename it `kernel.img` and put it on the sd-card. put that into the raspi.
        * establish a uart-connection to your raspi
        * run minicom (Bps/par/bits: 115200 8N1, no flow control)
        * send `kernel.srec` (as ascii)
        * you might have to press `g` (for go) to start
        * done.
