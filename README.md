# octopOS

In scope of Operating Systems WS2122 we, Fabian Jezuita and Paul Weiß, trapped a witty octopOS in a virtual portux armv4 computer to manage resources and, depending on "it", also other stuff.

No cephalopods were harmed in the making of this.

## getting started

This Walkthrough assumes this readme was cloned into `octopos/README.md`

1. run `make dependencies` to install the toolchain and `qemu` dependencies.
2. obtain the custom version of qemu  
e.g. from https://git.imp.fu-berlin.de/koenigl/qemu-portux.git
3. create a folder `octopos/links`
4. **GO INTO THAT FOLDER:**, `ln` works in mysterious ways: `cd links`
5. create a symlink to your `qemu` binary, should be in `qemu-build/arm-softmmu/qemu-system-arm` using `ln`:
```
$ ln -sT ~/qemu-build/arm-softmmu/qemu-system-arm qemu
```
6. build the project
```
$ make
```
7. Optional: run the project `make debug`  
in another terminal enter the displayed command to connect gdb  
To quit `qemu` enter CTRL+a followd by x into its terminal

## test kernel on andorra
This provides the possibility to test the kernel without having a local build of qemu.

1. build `kernel.elf` on your machine:
```
$ make all
```
2. test it on andorra by running:
```
$ sh andorra_tst.sh [FU-USERNAME]
```
