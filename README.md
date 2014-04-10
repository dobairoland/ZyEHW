# ZyEHW

ZyEHW is a joint hardware-software project for evolutionary design in the
Xilinx Zynq-7000 all programmable (AP) system-on-chip (SoC) platform. The name
ZyEHW is a concatenation of the first two letters of Zynq and the acronym for
evolvable hardware (EHW).

ZyEHW is the result of our ongoing research and intends to demonstrate our
scientific results. Some of these results were already published (you can find
a list of related publications in this file). More papers will follow soon.
ZyEHW does not contain the descriptions of our developed methods. Please read
our papers if you want to find out more about our methods.

Currently, ZyEHW is able to evolve *image filters* only. Our intention is to
include more case studies in the near future.

### Who can benefit from ZyEHW?

1. If you are interested in *image filtering* then you can create a complete
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

We recommend to follow the instructions in `README.md` of each folders. The
best way is visiting the folders and reading the instructions in the following
order.

1. Folder `hw` contains instructions and the source for generating the
hardware configuration, i.e. the configuration bit stream for the programmable
logic of Zynq-7000 AP SoC.

1. The software for the processor cores of Zynq-7000 AP SoC are in folder
`sw`.

1. Software tools for preprocessing test inputs and postprocessing test
outputs can be found in folder `tools`.

## Related publications

1. R. Dobai, L. Sekanina: [Image Filter Evolution on the Xilinx Zynq
Platform][ahs13], In: 2013 NASA/ESA Conference on Adaptive Hardware and
Systems, 164-171, 2013.

1. R. Dobai, L. Sekanina: [Towards Evolvable Systems Based on the Xilinx Zynq
Platform][ices13], In: 2013 IEEE International Conference on Evolvable Systems
(ICES), 89-95, 2013.

[ahs13]: http://dx.doi.org/10.1109/AHS.2013.6604241
[ices13]: http://dx.doi.org/10.1109/ICES.2013.6613287

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
