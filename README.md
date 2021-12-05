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
6. build the project, optinally define your board and type (defaults shown)
    ```
    $ make platform=portux build=release
    ```
7. run the project (on your local machine):
    ```
    $ make platform=portux build=release run
    ```
    In order to run the kernel on andorra, follow instructions below.

8. optional: run the project in debug mode
    ```
    $ make platform=portux build=debug run
    ```
    in another terminal enter the displayed command or run the debug script:
    ```
    $ make platform=portux build=debug debugger
    ```

**Note:** To quit `qemu` enter CTRL+a followd by x into its terminal.

## test kernel on andorra
This provides the possibility to test the kernel without having a local build of qemu.

1. build `kernel.elf` on your machine:
    ```
    $ make platform=portux build=debug all
    ```
2. test it on andorra by running:
    ```
    $ sh andorra_tst.sh [FU-USERNAME]
    ```

## Currently supported features on various platforms

| Platform | compile | flash + run | debugger |
| --- | --- | --- | --- |
| portux (in qemu) | yes | yes, with `run` | yes |
| pi-pico | yes | manual³ | yes¹ |
| pi-zero | yes | yes² | nope |

¹To run the code, you have to connect the pi-pico via SWD to openOCD and start the debugger pointed at the openOCD, like so:
```
make platform=pi-pico build=<you choose> debug_target=<openOCD host>:<openOCD port> debug_load=1 debugger
```
² consult corresponding [README](https://git.imp.fu-berlin.de/weip00/octopos/-/blob/main/boards/pi-zero/README.md)

³A standard uf2 file is produded, these cannot be flashed automatically.