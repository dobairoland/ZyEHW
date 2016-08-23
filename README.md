# ZyEHW

ZyEHW is a joint hardware-software project for evolutionary design in the
Xilinx Zynq-7000 all programmable (AP) system-on-chip (SoC). The name
ZyEHW is a concatenation of the first two letters of Zynq and the acronym for
evolvable hardware (EHW).

ZyEHW is the result of our ongoing research and intends to demonstrate our
scientific results. Some of these results were already published (you can find
a list of related publications in this file).
ZyEHW does not contain the descriptions of our developed methods. Please read
our papers if you want to find out more about our methods.

Currently, ZyEHW is able to evolve *image filters* only. Our intention is to
include more case studies in the near future.

## Who can benefit from ZyEHW?

1. If you are interested in *image filtering* then you can create by ZyEHW a
complete
hardware system for evolving image filters. Image filters are evolved based on
the provided training data (video sequence) and no knowledge of conventional
image filtering is required for setting up such a system. The hardware will
evolve image filters autonomously (without human intervention). The
documentation of ZyEHW will guide you how to achieve this but read our papers
for more background information.

1. If you want to know how to create your own *evolvable hardware system* then
you can analyze and reuse our codes (see our licensing).

1. If you are interested in *partial reconfiguration* at very low-level then
you can learn how to do reconfiguration of look-up tables. This kind of
reconfiguration is faster than the one supported by PlanAhead and Vivado.

1. If you want to know how to use *Xilinx Vivado* for developing complex
hardware systems then you can find our scripts for the automated use of this
tool.

Contact us if something is not clear and want a more detailed explanation (for
example more comments somewhere in the code). If you find ZyEHW useful then it
would be a nice way to support us by citations to our papers.

## Installing and using ZyEHW

We recommend to follow the instructions in the `README.md` files (each folder
has one). The best way to learn ZyEHW is visiting the folders and reading the
instructions in the following order.

1. Folder `hw` contains instructions and the source code for generating the
hardware configuration, i.e. the configuration bit stream for the programmable
logic of Zynq-7000 AP SoC.

1. The software for the processor cores of Zynq-7000 AP SoC are in folder
`sw`.

1. Software tools for preprocessing test inputs and postprocessing test
outputs can be found in folder `tools`.

## System requirements

1. Development board with Zynq-7000 AP SoC. ZyEHW was tested on a
[Zedboard](http://www.zedboard.org). You will need to connect your board with
your computer and setup a serial terminal program. Please read the Zedboard
Getting Started Guide in order to learn how to do that.

1. The latest version of [Xilinx](http://www.xilinx.com) Vivado Design Suite.
Please read the product documentation for installing and setting up the
software. A free Xilinx product license should be sufficient for this project.

1. We recommend to use Linux. ZyEHW was tested in [Centos
6.5](https://www.centos.org) running in
[Virtualbox](https://www.virtualbox.org).

1. Our pre- and postprocessing `tools` require [ffmpeg]( http://ffmpeg.org/)
for video processing. Other dependencies include [gcc](http://gcc.gnu.org),
[make](https://www.gnu.org/software/make), [libxml2](http://xmlsoft.org) and
[libtiff](http://www.remotesensing.org/libtiff).

## Related publications

1. R. Dobai, L. Sekanina: [Low-Level Flexible Architecture with Hybrid
Reconfiguration for Evolvable Hardware][trets15], In: ACM Transactions on
Reconfigurable Technology and Systems, vol. 8, no. 3, art. no. 20, 2015.

1. R. Dobai, K. Glette, J. Torresen, L. Sekanina: [Evolutionary Digital
Circuit Design with Fast Candidate Solution Establishment in Field
Programmable Gate Arrays][ices14], In: 2014 IEEE International Conference
on Evolvable Systems, 85-92, 2014.

1. R. Dobai: [Evolutionary On-line Synthesis of Hardware Accelerators for
Software Modules in Reconfigurable Embedded Systems][fpl14], In: 2014 24th
International Conference on Field Programmable Logic and Applications
(FPL), 2014.

1. R. Dobai, L. Sekanina: [Image Filter Evolution on the Xilinx Zynq
Platform][ahs13], In: 2013 NASA/ESA Conference on Adaptive Hardware and
Systems, 164-171, 2013.

1. R. Dobai, L. Sekanina: [Towards Evolvable Systems Based on the Xilinx Zynq
Platform][ices13], In: 2013 IEEE International Conference on Evolvable Systems
(ICES), 89-95, 2013.

[ahs13]: http://dx.doi.org/10.1109/AHS.2013.6604241
[ices13]: http://dx.doi.org/10.1109/ICES.2013.6613287
[fpl14]: http://dx.doi.org/10.1109/FPL.2014.6927455
[ices14]: http://dx.doi.org/10.1109/ICES.2014.7008726
[trets15]: http://dx.doi.org/10.1145/2700414

## Contact

You can contact me if you have any questions. My contacts are available on [my
web page](http://www.fit.vutbr.cz/~dobai/index.php.en).

## License

Copyright (C) 2014  Roland Dobai

ZyEHW is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ZyEHW is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available in file `COPYING`.
Further informations about the license can be found on the [GNU licenses web
page](http://www.gnu.org/licenses).
