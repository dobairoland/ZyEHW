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

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "frm.h"
#include "noise.h"
#include "video.h"

typedef enum {
        NOP,
        SALT_AND_PEPPER
} inoperator_t;

static int finalx = -1, finaly = -1;

static inoperator_t inop = NOP;
static int inop_val = 0;

static inline void print_help(const char *prg)
{
        printf("Usage: %s -i <input video> -o <output video> "
                        "[ -x <output x resolution> ] "
                        "[ -y <output y resolution> ] "
                        "[ -s <random seed> ] "
                        "[ -sp <salt and pepper %d-%d> ] "
                        "\n", prg, SALTPEPPER_MIN, SALTPEPPER_MAX);
}

static inline void apply_input_operator(frm_t *frm, int frmsize)
{
        int ret = 1;
        switch (inop) {
                case SALT_AND_PEPPER:
                        ret = apply_salt_pepper(frm, frmsize, inop_val);
                        break;
                case NOP:
                        break;
        }

        if (!ret)
                exit(EXIT_FAILURE);
}

void process_frame(const frm_t *frmin, int insize, int x, int y, frm_t *frmout)
{
        (void) x;
        (void) y;

        copy_frm(frmin, frmout, insize);
        apply_input_operator(frmout, insize);
}

int main(int argc, char *argv[])
{
        int i;
        int x = NOSCALE, y = NOSCALE;
        char *input = NULL, *output = NULL;
        unsigned int seed = time(NULL);

        for (i = 1; i < argc; ++i) {
                if ((strcmp(argv[i], "-i") == 0) && ((i+1) < argc)) {
                        input = argv[++i];
                } else if ((strcmp(argv[i], "-o") == 0) && ((i+1) < argc)) {
                        output = argv[++i];
                } else if ((strcmp(argv[i], "-x") == 0) && ((i+1) < argc)) {
                        x = (int) strtol(argv[++i], NULL, 10);
                } else if ((strcmp(argv[i], "-y") == 0) && ((i+1) < argc)) {
                        y = (int) strtol(argv[++i], NULL, 10);
                } else if ((strcmp(argv[i], "-s") == 0) && ((i+1) < argc)) {
                        seed = (unsigned int) strtol(argv[++i], NULL, 10);
                } else if ((strcmp(argv[i], "-sp") == 0) && ((i+1) < argc)) {
                        inop = SALT_AND_PEPPER;
                        inop_val = (int) strtol(argv[++i], NULL, 10);
                }
        }

        if ((input != NULL) && (output != NULL) && (x == NOSCALE || x > 0) &&
                        (y == NOSCALE || y > 0)) {
                int ret;

                srand(seed);
                printf("Seed: %d\n", seed);
                ret = video_input_pipe(input, output, x, y, &finalx, &finaly,
                                raw_video_output,
                                process_frame);

                return ret;
        }

        print_help(argv[0]);

        return EXIT_FAILURE;
}
