/*
 * Copyright (C) 2014 Roland Dobai
 *
 * This file is part of ZyEHW.
 *
 * ZyEHW is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * ZyEHW is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along
 * with ZyEHW. If not, see <http://www.gnu.org/licenses/>.
 */

#include "tif_handler.h"

#include "tiffio.h"

#define DPI             72.0
#define BYTE0           0
#define BYTE1           8
#define BYTE2           16
#define BYTE3           24

static inline int is_grayscale(uint32 pixel)
{
        return (TIFFGetR(pixel) == TIFFGetB(pixel)) &&
                (TIFFGetR(pixel) == TIFFGetG(pixel));
}

img_t *load_tif(const char *file, int *size, int *width)
{
        size_t i;
        img_t *img = NULL;
        TIFF *tif = TIFFOpen(file, "r");
        *size = 0;
        *width = 0;

        if (tif) {
                uint32 w, height;
                size_t npixels;
                uint32 *raster;

                TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
                TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
                *width = w;
                *size = npixels = w * height;
                raster = (uint32 *) _TIFFmalloc(npixels * sizeof(uint32));

                if (raster != NULL && (img = allocate_img(npixels)) != NULL) {
                        if (TIFFReadRGBAImage(tif, w, height, raster, 0)) {
                                for (i = 0; i < npixels; ++i) {
                                        if (is_grayscale(raster[i]))
                                                update_img(img, i,
                                                       TIFFGetR(raster[i]));
                                        else
                                                fprintf(stderr, "Pixel is not "
                                                                "grayscale");
                                }
                        }
                        _TIFFfree(raster);
                }
                TIFFClose(tif);
        }
        return img;
}

void finalize_tif(img_t *img)
{
        deallocate_img(img);
}

void write_tif(const char *file, const img_t *img, int size, int width)
{
        int i, j;
        const int raster_size = size >> 2; /* division by 4 -> uint32 has 4 bytes */
        TIFF *tif = TIFFOpen(file, "w");

        if (tif) {
                uint32 *raster = (uint32 *) _TIFFmalloc(size * sizeof(uint32));

                if (!raster)
                        return;

                TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
                TIFFSetField(tif, TIFFTAG_IMAGELENGTH, size/width);
                TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
                TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
                TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, size/width);

                TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPRIGHT);

                TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
                TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

                TIFFSetField(tif, TIFFTAG_XRESOLUTION, DPI);
                TIFFSetField(tif, TIFFTAG_YRESOLUTION, DPI);
                TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

                for (i = 0, j = size; i < raster_size; ++i) {
                        const img_t byte0 = get_img(img, --j);
                        const img_t byte1 = get_img(img, --j);
                        const img_t byte2 = get_img(img, --j);
                        const img_t byte3 = get_img(img, --j);

                        raster[i] = (byte0 << BYTE0) |
                                        (byte1 << BYTE1) |
                                        (byte2 << BYTE2) |
                                        (byte3 << BYTE3);
                }

                TIFFWriteRawStrip(tif, 0, raster, size);

                _TIFFfree(raster);
                TIFFClose(tif);
        }
}
