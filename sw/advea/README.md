# Software module of ZyEHW for the advanced evolutionary algorithm

The `sw/advea/cpu0` folder contains the code for the processor of Zynq-7000 AP
SoC. The hardware project previously exported the necessary information for
the Xilinx Software Development Kit (SDK). Open the SDK by running the
following command from the same terminal where the Xilinx setting file was
sourced.
```sh
xsdk -workspace ../hw/prj/prj.sdk/SDK/SDK_Export &
```

Create a Hardware Platform Specification based on
`hw/prj/prj.sdk/SDK/SDK_Export/hw/dsgn.xml` and name it `hw_platform_0`.

Create a new Xilinx Application Project. Set `cpu0` for the project name and
make sure that `hw_platform_0`, `ps7_cortexa9_0` and `standalone` options
are selected.

Link the codes form `sw/vidproc/cpu0` into the SDK projects. This can be
achieved by running the following commands.
```sh
pushd ../hw/prj/prj.sdk/SDK/SDK_Export/cpu0
rm -r src
ln -s ../../../../../../sw/vidproc/cpu0 src
popd
```

Connect Zedboard to your computer and start the serial terminal program. A lot
of data will be sent through the serial link so it might be a good idea to
save it into a file. For example, `minicom -C /tmp/log.txt` can achieve this
if you are using `minicom`.

Configure the programmable logic by the previously generated
`hw/prj/prj.runs/impl_1/zyehw.bit` bit stream. Start `cpu0`.

## What does CPU0 do?

CPU0 sends the training images into the programmable logic and then controls
the evolution. It sends each candidate filter into the programmable logic,
waits while the fitness value is computed and reads the fitness value back.
This fitness value guides the search. Consequently, new candidate filters are
generated and their fitness are determined. The evolution continues until a
desired number of generations of candidate solutions is processed.

## What will be transferred through the terminal?

You will receive something very similar in your terminal program:
```
CPU begins...
Run 0 is finished in 20 seconds (VRC: 2, DPR: 8, CGP: 5, seed: 0x100772).
...
<?xml version="1.0"?>
<cgp col="8" row="4" bit="8">
...
</cgp>

Median of 30 runs:
(0, 98423)
(107, 97547)
...
(99558, 11788)

Lower quartile of 30 runs:
(0, 98423)
(40, 94176)
...
(99881, 11005)

Upper quartile of 30 runs:
(0, 98423)
(432, 88493)
...
(98736, 13259)
CPU ends.
```

The most important part of the received transfer is the content of `cgp` which
describes the best evolved solution. You will learn in `tools/README.md` how
to process the content of `cgp`. ZyEHW performs multiple runs of evolution and
prints also the fitness values for statistical evaluation.
