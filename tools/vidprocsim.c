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
#include <time.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "frm.h"
#include "noise.h"
#include "video.h"
#include "cgp.h"
#include "operators.h"
#include "xml.h"
#include "tif_handler.h"

typedef enum {
        NOP,
        SALT_AND_PEPPER
} inoperator_t;

typedef enum {
        VOID,
        EDGE,
        MEDIAN
} outoperator_t;

static int finalx = -1, finaly = -1;

static inoperator_t inop = NOP;
static int inop_val = 0;
static outoperator_t outop = VOID;

static frm_t *frmprepr = NULL;
static frm_t *frmdeterm = NULL;
static frm_t *frmevo = NULL;

static char *dir = NULL;

static inline void print_help(const char *prg)
{
        printf("Usage: %s -i <input video> -o <output video> "
                        "-f <filter xml> "
                        "-d <output dir> "
                        "[ -x <output x resolution> ] "
                        "[ -y <output y resolution> ] "
                        "[ -s <random seed> ] "
                        "[ -sp <salt and pepper %d-%d> ] "
                        "[ -edge] "
                        "[ -med ]\n", prg, SALTPEPPER_MIN, SALTPEPPER_MAX);
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

static inline void apply_output_operator(const frm_t *frmin, frm_t *frmout, int
        frmsize)
{
        switch (outop) {
                case MEDIAN:
                        apply_median(finalx, finaly, frmin, frmout);
                        break;
                case EDGE:
                        apply_edge(finalx, finaly, frmin, frmout);
                        break;
                case VOID:
                        apply_void(frmout, frmsize);
                        break;
        }
}

static void join_frames(frm_t *frmjoined, const frm_t *frm1, const
                frm_t *frm2, const frm_t *frm3, const frm_t *frm4)
{
        int x, y, a, b, j;

        a = b = j = 0;

        for (y = 0; y < finaly; ++y) {
                for (x = 0; x < finalx; ++x)
                        update_frm(frmjoined, j++, get_frm(frm1, a++));

                for (x = 0; x < finalx; ++x)
                        update_frm(frmjoined, j++, get_frm(frm2, b++));
        }

        a = b = 0;

        for (y = 0; y < finaly; ++y) {
                for (x = 0; x < finalx; ++x)
                        update_frm(frmjoined, j++, get_frm(frm3, a++));

                for (x = 0; x < finalx; ++x)
                        update_frm(frmjoined, j++, get_frm(frm4, b++));
        }
}

static void allocate_frames(int frmsize)
{
        if (!frmprepr || !frmdeterm || !frmevo) {
                frmprepr = allocate_frm(frmsize);
                frmdeterm = allocate_frm(frmsize);
                frmevo = allocate_frm(frmsize);

                if (!frmprepr || !frmdeterm || !frmevo) {
                        fprintf(stderr, "Cannot allocate frames!\n");
                        exit(EXIT_FAILURE);
                }
        }
}

double compute_psnr(const frm_t *frmref, const frm_t *frm, int size)
{
        int i;
        long acc = 0;
        const double max_val = ((int) FRM_MAX) * ((int) FRM_MAX);

        for (i = 0; i < size; ++i) {
                const int pix = get_frm(frm, i);
                const int refpix = get_frm(frmref, i);
                const int diff = pix - refpix;

                acc += diff * diff;
        }

        return acc == 0 ? 0 : 10*log10(max_val*size/((double) acc));
}

void process_frame(const frm_t *frmin, int insize, int x, int y, frm_t *frmout)
{
        static int vframe = 0;
        static char file[PATH_MAX];
        fitness_t fitness;
        quality_t comp_q;
        const cgp_t *cgp = xml_parse(vframe, &fitness);

        allocate_frames(insize);

        copy_frm(frmin, frmprepr, insize);
        apply_input_operator(frmprepr, insize);
        apply_output_operator(frmprepr, frmdeterm, insize);

        comp_q = cgp_compute_fitness(cgp,
                        frmdeterm, /* reference */
                        frmprepr, /* input with noise */
                        frmevo, /* output which was corrected */
                        insize, x, y);

        snprintf(file, PATH_MAX, "%s/%.6d_in.tiff", dir, vframe);

        if (!write_tif(file, frmin, insize, x)) {
                fprintf(stderr, "Cannot write image output!\n");
                exit(-1);
        }

        snprintf(file, PATH_MAX, "%s/%.6d_%.2f_prepr.tiff", dir, vframe,
                        comp_q.psnr_bad);

        if (!write_tif(file, frmprepr, insize, x)) {
                fprintf(stderr, "Cannot write image output!\n");
                exit(-1);
        }

        snprintf(file, PATH_MAX, "%s/%.6d_%.2f_determ.tiff", dir, vframe,
                        compute_psnr(frmin, frmdeterm, insize));

        if (!write_tif(file, frmdeterm, insize, x)) {
                fprintf(stderr, "Cannot write image output!\n");
                exit(-1);
        }

        snprintf(file, PATH_MAX, "%s/%.6d_%.2f_evolv.tiff", dir, vframe,
                        comp_q.psnr_filtered);

        if (!write_tif(file, frmevo, insize, x)) {
                fprintf(stderr, "Cannot write image output!\n");
                exit(-1);
        }

        join_frames(frmout,
                        frmin,
                        frmprepr,
                        frmdeterm,
                        frmevo);

        ++vframe;
}

static int dir_exists(const char *path)
{
        struct stat s;

        if (!path)
                return 0;

        if (stat(path, &s) != 0) {
                perror("stat");
                return 0;
        }

        if ((s.st_mode & S_IFMT) != S_IFDIR) {
                fprintf(stderr, "%s is not a directory!\n", path);
                return 0;
        }

        return 1;
}

int main(int argc, char *argv[])
{
        int i;
        int x = NOSCALE, y = NOSCALE;
        char *input = NULL, *output = NULL;
        char *filterxml = NULL;
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
                } else if ((strcmp(argv[i], "-f") == 0) && ((i+1) < argc)) {
                        filterxml = argv[++i];
                } else if ((strcmp(argv[i], "-d") == 0) && ((i+1) < argc)) {
                        dir = argv[++i];
                } else if (strcmp(argv[i], "-med") == 0) {
                        outop = MEDIAN;
                } else if (strcmp(argv[i], "-edge") == 0) {
                        outop = EDGE;
                }
        }

        if ((input != NULL) && (output != NULL) && (x == NOSCALE || x > 0) &&
                        (y == NOSCALE || y > 0) && (filterxml != NULL) &&
                        dir_exists(dir)) {
                int ret;

                srand(seed);
                printf("Seed: %d\n", seed);

                xml_start_parse(filterxml);

                ret = video_input_pipe(input, output, x, y, &finalx, &finaly,
                                video_output_pipe,
                                process_frame);

                xml_end_parse();

                deallocate_frm(frmprepr);
                deallocate_frm(frmdeterm);
                deallocate_frm(frmevo);

                return ret;
        }

        print_help(argv[0]);

        return EXIT_FAILURE;
}
