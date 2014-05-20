# Software module of ZyEHW (video processing case study)

The `sw/vidproc/cpu0` and `sw/vidproc/cpu1` folders contain the code for the
processors of
Zynq-7000 AP SoC. The hardware project previously exported the necessary
information for the Xilinx Software Development Kit (SDK). Open the SDK by
running the following command (from the same terminal where the Xilinx
setting file was sourced).
```sh
xsdk -workspace ../hw/prj/prj.sdk/SDK/SDK_Export &
```

Create a Hardware Platform Specification based on
`hw/prj/prj.sdk/SDK/SDK_Export/hw/dsgn.xml` and name it `hw_platform_0`.

Create a new Xilinx Application Project. Set `cpu0` for the project name and
make sure that `hw_platform_0`, `ps7_cortexa9_0` and `standalone` options
are selected. Include `xilffs` library into the board support package
`cpu0_bsp`.

Create another Xilinx Application Project. Set the name of the project to
`cpu1` and select `ps7_cortexa9_1`. The other options should be the same as for
`cpu0`.

Link the codes form `sw/vidproc/cpu0` and `sw/vidproc/cpu1` into the SDK
projects.
This can be achieved by running the following commands.
```sh
pushd ../hw/prj/prj.sdk/SDK/SDK_Export/cpu0
rm -r src
ln -s ../../../../../../sw/vidproc/cpu0 src
popd
pushd ../hw/prj/prj.sdk/SDK/SDK_Export/cpu1
rm -r src
ln -s ../../../../../../sw/vidproc/cpu1 src
popd
```

Usually the whole address space is occupied by one processor. We want to use
both processors in parallel, therefore we need to make some adjustments. And
the
stack and heap sizes should be increased also.

Generate the linker script for `cpu0`. Use the default settings but modify the
heap and stack size to `0x2000` and decrease the mapped address size of the
external memory to `0x0FF80000`. This can be done either in GUI or by directly
editing the generated `hw/prj/prj.sdk/SDK/SDK_Export/cpu0/src/lscript.ld`
linker script. These are the relevant lines of the file:
```
_STACK_SIZE = DEFINED(_STACK_SIZE) ? _STACK_SIZE : 0x2000;
_HEAP_SIZE = DEFINED(_HEAP_SIZE) ? _HEAP_SIZE : 0x2000;
...
ps7_ddr_0_S_AXI_BASEADDR : ORIGIN = 0x00100000, LENGTH = 0x0FF80000
```

Generate the linker script for `cpu1` similarly. Modify the heap size to
0x2000 and
stack size to `0x8000000`. Decrease the mapped address size to `0x0FF80000`
and set the base address to `0x10080000`. The relevant lines of the
`hw/prj/prj.sdk/SDK/SDK_Export/cpu1/src/lscript.ld` file should look like
as it follows.
```
_STACK_SIZE = DEFINED(_STACK_SIZE) ? _STACK_SIZE : 0x2000;
_HEAP_SIZE = DEFINED(_HEAP_SIZE) ? _HEAP_SIZE : 0x8000000;
...
ps7_ddr_0_S_AXI_BASEADDR : ORIGIN = 0x10080000, LENGTH = 0x0FF80000
```

Connect Zedboard to your computer and start the serial terminal program. A lot
of data will be sent through the serial link so it might be a good idea to
save it into a file. For example, `minicom -C /tmp/log.txt` can achieve this
if you are using `minicom`.

Configure the programmable logic by the previously generated
`hw/prj/prj.runs/impl_1/zyehw.bit` bit stream. Start `cpu0` and `cpu1` (it is
important to start them in this order).

## What does CPU0 do?

CPU0 reads video frames from the SD card, processes them and sends them into
the programmable logic. You will learn in `tools/README.md` how to prepare the
data for the SD card.

## What does CPU1 do?

CPU1 controls the evolution. It sends each candidate filter into the
programmable logic, waits while the fitness value is computed and reads the
fitness value back. This fitness value guides the search. Consequently, new
candidate filters are generated and their fitness are determined. The
evolution continues until the other processor sends new video frames into the
programmable logic.

## What will be transferred through the terminal?

You will receive something very similar in your terminal program which will be
very long and the transfer will take 10-20 minutes:
```
CPU 0 begins...
CPU 1 begins...
Run 0 is finished.
<?xml version="1.0"?>
<cgp_for_frames col="8" row="4" bit="8">
...
</cgp_for_frames>

Here are the fitness values for all frames in run 0:
64614
66346
...
71075
71232
CPU 1 ends.
```

The most important part of the received transfer is the content of
`cgp_for_frames` which describes the best evolved candidate solutions for each
video frame. Only some of the candidate solutions are transferred for longer
videos (in order to reduce the transfer time). You will learn in
`tools/README.md` how to process the content of `cgp_for_frames`. ZyEHW can
perform multiple runs on the same video sequence and print the fitness values
of the best candidate filters (for each frame and in each run). This can be
used for statistical evaluation.
