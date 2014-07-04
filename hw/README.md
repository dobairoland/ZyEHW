# Hardware module of ZyEHW

The hardware which will be implemented in the programmable logic of Zynq-7000
AP SoC is described by the files located in the `hw/hdl` folder. The
processors of Zynq-7000 AP SoC will communicate with the designed hardware
through the AXI bus. Training data will be sent into the hardware. The
training data is a sequence of input video frames and a
sequence of expected filter outputs. Candidate filters will be established
by partial reconfiguration and writing into the configuration registers. The
candidate filters can be evaluated in the programmable logic and their fitness
computed and stored in registers. Please read our papers for more information.

This file will guide you how to generate the bit stream for configuring the
programmable logic. First of all, make sure that your current directory is
`hw` and the Vivado settings file is sourced (the path may vary depending on
your installation).
```sh
cd hw
. /opt/Xilinx/Vivado/2013.4/settings64.sh
```

The `create.tcl` file is provided for setting up the project. If you do not
use a Zedboard then you need to change the development board in this file. The
project will be placed in the `hw/prj` folder after the following command was
invoked.
```sh
vivado -mode tcl -source create.tcl
```

If it is more convenient for you then you can open the graphical user
interface (GUI) by running `start_gui` from the Vivado console.

The generation of the hardware configuration bit stream can be initiated by
running `source rerun.tcl` from the Vivado console. This will take
approximately 15-30 minutes depending on the performance of your computer.
Based on your preferences you can complete this task manually or from the GUI.

Please make sure that all timing constraints are met. You can do this by
checking the logs, the timing reports or just making sure that *0 Critical
Warnings* were encountered during the routing.

You can close Vivado now and proceed to the `sw` directory for setting up the
software for the processors of Zynq-7000 AP SoC.
