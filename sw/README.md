# Software module of ZyEHW

The `sw/vidproc` folder contains the code for the video processing case study.
This case study demonstrates how to evolve image filters. One processor core
processes video frames and provides the training data for the evolution. The
other processor core manages the evolution and uses the programmable logic for
establishing and evaluating candidate solutions.

The `sw/advea` folder contains the code for the advanced evolutionary
algorithm which was developed in order to reduce the reconfiguration time by
proposing a new mutation operator. The case study evolves images filters with
better quality than before in the same time.

More details are given in the `README.md` files of each subfolder.
