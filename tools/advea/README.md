# Tools of ZyEHW for the advanced evolutionary algorithm

The input for ZyEHW is the training image. The output is the description of
the best evolved filter transferred through the serial link. We developed a
series of tools for processing these input and output. Please note that these
tools work for the given purpose but were not optimized for speed.

The tools can be compiled by running `make` from the `tools/advea` folder.

## Preparing training inputs

You will need a gray scale image of resolution `128x128` in the TIF format.
Run the following command
```sh
./img_manip -i lena128.tif -o lenaout.tif -sp 5
```
where `lena128.tif` is the input image and `lenaout.tif` is the output image
which will be soiled with 5% salt-and-pepper noise. Additionally,
`correct.txt`, `correct.c`, `corrupt.txt` and `corrupt.txt` files are also
generated. The `txt` files will be used for generating the filtered image
based on the evolved filter. The contents of the `c` files can be used for
replacing the training inputs and outputs: `TRAINING_INPUT` and
`TRAINING_OUTPUT` in the file `sw/advea/cpu0/train_data.c`.

File `sw/advea/cpu0/train_data.c` contains several training images and you do
not have to prepare your own. The Lena image is used for default, and
`lena_correct.txt` and `lena_corrupt.txt` are available for generating the
filtered image.

## Generating the filtered image

Locate the following part of the data received from the serial link and save
it as `filter.xml`.
```xml
<?xml version="1.0"?>
<cgp col="8" row="4" bit="8">
...
</cgp>
```

Run the following command in order to check the validity of the transferred
data and generate the filtered image using the evolved filter.
```sh
./out_proc -g lena_correct.txt -b lena_corrupt.txt -x filter.xml
```

The check should pass and you will see something similar:
```
The parsed and computed fitness are equal: 10183 (0x27c7)
```

You can inspect the generated `corrected.tif` file and see how the evolved
filter is able to reduce the noise in the `corrupt.tif` image.
