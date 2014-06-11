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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "img.h"
#include "tif_handler.h"
#include "cgp.h"
#include "xml.h"

/* +2 is endline and terminating character */
#define MAX_IMG_RESOLUTION (64+2)

#define CORRECT_TIF "correct.tif"
#define CORRUPT_TIF "corrupt.tif"
#define CORRECTED_TIF "corrected.tif"

static inline void print_help(const char *prg)
{
        printf("Usage: %s -g <correct.rom.dat> -b <corrupt.rom.dat> "
                        "-x <xml>\n", prg);
}

static img_t parse_pixel(const char *buffer)
{
        img_t val = 0;
        int end, i, mask;

        for (end = 0; buffer[end] == '0' || buffer[end] == '1'; ++end);

        for (i = (end - 1), mask = 1; i >= 0; --i, mask <<= 1) {
                if (buffer[i] == '1')
                        val += mask;
        }

        return val;
}

static int load_rom(const char *file, img_t **img)
{
        static char buffer[MAX_IMG_RESOLUTION];
        int size = 0, i;
        char c;
        FILE *f = fopen(file, "r");

        if (f != NULL) {
                do {
                        c = fgetc(f);

                        if (c == '\n')
                                ++size;

                } while (c != EOF);

                if (c != '\n' && size != 0)
                        ++size;

                rewind(f);

                *img = allocate_img(size);

                for (i = 0; i < size; ++i) {
                        fgets(buffer, MAX_IMG_RESOLUTION, f);

                        update_img(*img, i, parse_pixel(buffer));
                }
        }

        fclose(f);

        return size;
}

static int execute(const char *correct, const char *corrupt, const char *xml)
{
        int size;
        img_t *good_img, *bad_img, *corrected_img;
        cgp_t *cgp = cgp_allocate();
        fitness_t fit_comp, fit_pars = 0;

        size = load_rom(correct, &good_img);

        if (load_rom(corrupt, &bad_img) != size) {
                fprintf(stderr, "The size of ROM images should be the same!");
                return EXIT_FAILURE;
        }

        corrected_img = allocate_img(size);

        xml_parse(xml, &fit_pars, cgp);
        fit_comp = cgp_compute_fitness(cgp, good_img, bad_img, corrected_img,
                        size);

        write_tif(CORRECT_TIF, good_img, size, sqrt(size));
        printf("Correct image has been written as %s.\n", CORRECT_TIF);

        write_tif(CORRUPT_TIF, bad_img, size, sqrt(size));
        printf("Corrupt image has been written as %s.\n", CORRUPT_TIF);

        write_tif(CORRECTED_TIF, corrected_img, size, sqrt(size));
        printf("Corrected image has been written as %s.\n", CORRECTED_TIF);

        if (fit_comp == fit_pars)
                printf("The parsed and computed fitness are equal: %d (0x%x)\n",
                                fit_pars, fit_pars);
        else
                fprintf(stderr, "ERROR: the XML fitness is %d (0x%x) and the "
                                "computed one is %d (0x%x)\n", fit_pars,
                                fit_pars, fit_comp, fit_comp);

        deallocate_img(good_img);
        deallocate_img(bad_img);
        deallocate_img(corrected_img);
        cgp_deallocate(cgp);

        return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
        int i;
        char *correct = NULL, *corrupt = NULL, *xml = NULL;

        for (i = 1; i < argc; ++i) {
                if ((strcmp(argv[i], "-g") == 0) && ((i+1) < argc)) {
                        correct = argv[++i];
                } else if ((strcmp(argv[i], "-b") == 0) && ((i+1) < argc)) {
                        corrupt = argv[++i];
                } else if ((strcmp(argv[i], "-x") == 0) && ((i+1) < argc)) {
                        xml = argv[++i];
                }
        }

        if ((correct != NULL) && (corrupt != NULL) && (xml != NULL))
                return execute(correct, corrupt, xml);

        print_help(argv[0]);

        return EXIT_FAILURE;
}
