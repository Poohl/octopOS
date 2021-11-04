# How the raspberry pi pico boots:

This explanation will ignore what happnes upon failure and other special modes.

## Step 1: Boot Rom

The integrated cpu starts code execution at `0x0`, to wich 16kb of boot-rom are mapped.
It does some inital hardware setup and halts the second core.
This boot-rom will then load the first `256` bytes of flash located at `0x10000000` into sram5 (starting from `0x20041000`).
The last 4 bytes of these are assumed to be a CRC32 of the previous 252 and a integety check is performed (see RP2040 datasheet for details).
The `ld` register is cleared (!).
once sucessfull the code is executed in thumb mode.

## Step 2: Bootloader

boot_stage2 bootloader has to be exacly 252 bytes of thumb code, it's only purpose is to
- eiher set up the flash controller (XIP) to enable in place execution (the default)
- or load all flash into sram via whatever mode the XIP is in (you have to use a different one for this).
And make that execute.
After this is done the `ld` register is inspected:
- If nonzero, something called the bootloader: just return, that is `bx ld`
- If zero, execution started because of hardware reset:
  * the vector table is assumed to be at the end of the bootloader, that is at `0x100` or `256`
  * the first two cpu words after the Bootloader, that is `0x100` and `0x104` are use as main stack pointer (`msp`) and entry point.  
Here the entry point adresses lsb has a special function: if set the destination code is interpreted as thumb code (instead of arm).  
Because the code used here is intresting:
```
    ldr r0, =(XIP_BASE + 0x100) // load adress of first cpuword after bootloader
    ldr r1, =(PPB_BASE + M0PLUS_VTOR_OFFSET) // load address of register holding vector table offset, effectively *sp
    str r0, [r1] // store first address after bootloader as vector table offset
    ldmia r0, {r0, r1} // load first two cpuwords of vecor table into r0 and r1
    msr msp, r0 // use first one as sp
    bx r1 // use seconf one as _start
```

## Step 3: Your job
The first two cpuwords (each 4 bytes) have to be the SP and _start (with the bit thing). From then on you have to run the show.

