# Tools of ZyEHW

The input for ZyEHW is the video sequence stored in the card. The output is
the description of the best evolved filters transferred through the serial
link. We developed a series of tools for processing these inputs and outputs.
Please note that these tools work for the given purpose but were not optimized
for speed.

You will learn in this file how to prepare the training inputs, check the
validity of the transferred data and generate the filtered video.

The tools can be compiled by running `make` from the `tools` folder.

## Preparing training inputs

You will need a video sequence. Use something short in your first
experiment, e.g. [foreman](http://media.xiph.org) which has only 300 video
frames. If you choose something else then you will need to make modifications
in the `sw/cpu0/main.c` file (`FRAMEWIDTH`, `FRAMEHEIGHT` and `FRAMECOUNT`
need to be adjusted).

Rescale and crop the video to `128x128` resolution. This can be achieved for
example by the following command.
```sh
ffmpeg -i videoin -vf "crop=w=in_h,scale=128:128" videoout.y4m
```

Generate the video in a simple format which can be easily processed in CPU0
(without the need of video processing libraries) and insert 5% salt-and-pepper
noise into it.
```sh
./testvid -i videoout.y4m -o i.dat -sp 5
```
If you want to evolve an edge detector instead of a noise filter then run the
command without the `-sp 5` option (and you also need to enable edge detection
in the `sw/cpu0/main.c` file). You can check the result by playing `i.dat.y4m`
which was also generated.

Copy `i.dat` to the SD card (and rename it to `oi.dat` for edge detection).
CPU0 will use this file in order to generate the training input for the
evolution.

## Checking the transferred data

Locate the following part of the data received from the serial link and save
it as `filter.xml`.
```xml
<?xml version="1.0"?>
<cgp_for_frames col="8" row="4" bit="8">
...
</cgp_for_frames>
```

Run the following command for checking the validity of the evolution (use the
`-edge` option instead of `-med` when doing edge detection).
```sh
./check -i i.dat -f filter.xml -med
```
This will compare the hardware-generated results with the software-based
simulations. You should receive the following response.
```
300 frames were checked and everything seem to be OK!
```

## Generating the filtered video

Use the following command in order to generate the filtered video (use `-edge`
instead of `-sp 5 -med` for edge detection).
```sh
./vidprocsim -i videoin -f filter.xml -o demonstration.y4m \
    -sp 5 -med -d /tmp/dir
```

The `demonstration.y4m` video will show and compare the results of the best
evolved filter for each video frame. You will see simultaneously four video
sequences when playing this video:

1. The video in northwestern corner will be the original video without any
noise, i.e. `videoin`.

1. The northeastern video will contain the noise if the `-sp` option was used.

1. The southwestern video will show the processed video using a conventional
method (median filter for `-med` option and Sobel edge detector for
`-edge`).

1. The filtered video using the evolved filters will be in the southeastern
corner.
