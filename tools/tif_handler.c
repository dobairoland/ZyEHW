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

int write_tif(const char *file, const frm_t *img, int size, int width)
{
        int i, j;
        const int raster_size = size >> 2; /* uint32 has 4 bytes */
        TIFF *tif = TIFFOpen(file, "w");

        if (tif) {
                uint32 *raster = (uint32 *) _TIFFmalloc(size * sizeof(uint32));

                if (!raster)
                        return 0;

                TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
                TIFFSetField(tif, TIFFTAG_IMAGELENGTH, size/width);
                TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
                TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
                TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, size/width);

                TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_BOTRIGHT);

                TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
                TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

                TIFFSetField(tif, TIFFTAG_XRESOLUTION, DPI);
                TIFFSetField(tif, TIFFTAG_YRESOLUTION, DPI);
                TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

                for (i = 0, j = size; i < raster_size; ++i) {
                        const frm_t byte0 = get_frm(img, --j);
                        const frm_t byte1 = get_frm(img, --j);
                        const frm_t byte2 = get_frm(img, --j);
                        const frm_t byte3 = get_frm(img, --j);

                        raster[i] = (byte0 << BYTE0) |
                                        (byte1 << BYTE1) |
                                        (byte2 << BYTE2) |
                                        (byte3 << BYTE3);
                }

                TIFFWriteRawStrip(tif, 0, raster, size);

                _TIFFfree(raster);
                TIFFClose(tif);

                return 1;
        }

        return 0;
}
