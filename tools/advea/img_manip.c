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
#include <time.h>
#include "tif_handler.h"
#include "img.h"

#define SP_NOISE1 0x00
#define SP_NOISE2 0xFF

#define BYTEBITS 8

#define ROM_CORRECT "correct.txt"
#define ROM_CORRUPT "corrupt.txt"
#define C_CORRECT "correct.c"
#define C_CORRUPT "corrupt.c"

typedef enum {NOP, SALT_PEPPER} option;

static void print_help(const char *prg)
{
        printf("Usage: %s [OPTIONS] -i <input> -o <output>\n", prg);
        printf("\tOptions are:\n");
        printf("\t\t-sp N - salt and pepper noise N%%\n");
}

static void printf_bin(FILE *f, img_t c)
{
        int mask, i;

        for (i = 0, mask = (1<<(BYTEBITS-1)); i < BYTEBITS; ++i, mask >>= 1) {
                putc(c & mask ? '1' : '0', f);
        }
}

static void save_rom_file(const img_t *img, int size, const char *file)
{
        int i;
        FILE *f = fopen(file, "w");

        if (!f)
                return;

        for (i = 0; i < size; ++i) {
                printf_bin(f, get_img(img, i));
                putc('\n', f);
        }

        fclose(f);
        printf("ROM file %s has been written.\n", file);
}

static void save_c_file(const img_t *img, int size, const char *file)
{
        int i;
        FILE *f = fopen(file, "w");

        if (!f)
                return;

        for (i = 0; i < size; ++i) {
                fprintf(f, "0x%X, \\\n", get_img(img, i));
        }

        fclose(f);
        printf("C file %s has been written.\n", file);
}

static int salt_and_pepper(const char *in, const char *out, int noise_ratio)
{
        const int noise_arr[] = {SP_NOISE1, SP_NOISE2};
        int size = 0, width = 0, i;
        img_t *img = load_tif(in, &size, &width);
        int noisenum = (int)((size * noise_ratio) / (float) 100);

        save_rom_file(img, size, ROM_CORRECT);
        save_c_file(img, size, C_CORRECT);

        srand(time(NULL));

        for (i = 0; i < noisenum; ++i)
                update_img(img,
                                rand() % size,
                                noise_arr[
                                        rand() %
                                        (sizeof(noise_arr)/sizeof(noise_arr[0]))
                                        ]);

        save_rom_file(img, size, ROM_CORRUPT);
        save_c_file(img, size, C_CORRUPT);

        write_tif(out, img, size, width);

        finalize_tif(img);

        return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
        int i;
        int noise = 0;
        char *infile = NULL, *outfile = NULL;
        option op = NOP;

        for (i = 1; i < argc; ++i) {
                if((strcmp(argv[i], "-sp") == 0) && ((i+1) < argc)) {
                        noise = atoi(argv[++i]);

                        if(op == NOP && noise > 0 && noise < 100) {
                                op = SALT_PEPPER;
                        }
                        else {
                                op = NOP;
                                break;
                        }
                } else if ((strcmp(argv[i], "-i") == 0) && ((i+1) < argc)) {
                        infile = argv[++i];
                } else if ((strcmp(argv[i], "-o") == 0) && ((i+1) < argc)) {
                        outfile = argv[++i];
                } else {
                        op = NOP;
                        break;
                }
        }

        if ((infile != NULL) && (outfile != NULL)) {
                switch(op) {
                        case SALT_PEPPER:
                                return salt_and_pepper(infile, outfile, noise);
                        default:
                                break;
                }
        }

        print_help(argv[0]);
        return EXIT_FAILURE;
}
