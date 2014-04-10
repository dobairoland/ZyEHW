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

#include "cgp.h"
#include "operators.h"
#include "frm.h"
#include "xml.h"

#define DEFAULT_SIZE            (128*128)
#define DEFAULT_X               427
#define DEFAULT_Y               240

typedef enum {
        VOID,
        EDGE,
        MEDIAN
} outoperator_t;

static outoperator_t outop = VOID;

static inline void print_help(const char *prg)
{
        printf("Usage: %s -i <input video> "
                        "-f <filter xml> "
                        "[-s <training size as edge * edge> ]"
                        "[ -x <x resolution> ] "
                        "[ -y <y resolution> ] "
                        "[-med]"
                        "[-edge]"
                        "\n", prg);
}

static inline void apply_output_operator(const frm_t *frmin, frm_t *frmout,
                int x, int y)
{
        switch (outop) {
                case MEDIAN:
                        apply_median(x, y, frmin, frmout);
                        break;
                case EDGE:
                        apply_edge(x, y, frmin, frmout);
                        break;
                case VOID:
                        break;
        }
}

static inline void clip_frame(const frm_t *frmin, frm_t *frmout, int origsize,
                int origx, int origy, int newedge)
{
        int i;
        int j = 0;

        for (i = 0; i < origsize; ++i) {
                const div_t q = div(i, origx);

                if ((q.quot < (origy-newedge)/2) ||
                (q.quot >= (origy-newedge)/2+newedge) ||
                (q.rem < (origx-newedge)/2) ||
                (q.rem >= (origx-newedge)/2+newedge))
                        continue;

                frmout[j++] = frmin[i];
        }
}

static inline int process(const char *input, const char *xml, int size,
                int x, int y)
{
        FILE *vfile;
        int vframe;
        const int edge = sqrt(size);
        const int fullsize = x * y;
        frm_t *frmin = allocate_frm(fullsize);
        frm_t *frminclipped = allocate_frm(size);
        frm_t *frmdeterm = allocate_frm(fullsize);
        frm_t *frmdetclipped = allocate_frm(size);
        frm_t *frmevo = allocate_frm(size);

        if (!frmin || !frmdeterm || !frmevo || !frminclipped ||
                        !frmdetclipped) {
                fprintf(stderr, "Cannot allocate frames!\n");
                return EXIT_FAILURE;
        }

        if (!(vfile = fopen(input, "rb"))) {
                perror("Cannot open video input");
                return EXIT_FAILURE;
        }

        xml_start_parse(xml);

        for (vframe = 0;
        fread(frmin, sizeof(frm_t), fullsize, vfile) == ((size_t) fullsize);
        ++vframe) {
                fitness_t fitness;
                quality_t quality;
                const cgp_t *cgp = xml_parse(vframe, &fitness);

                if (!is_cgp_for_frame(vframe))
                        continue;

                apply_output_operator(frmin, frmdeterm, x, y);

                clip_frame(frmin, frminclipped, fullsize, x, y, edge);
                clip_frame(frmdeterm, frmdetclipped, fullsize, x, y, edge);

                quality = cgp_compute_fitness(cgp,
                                frmdetclipped, /* reference */
                                frminclipped, /* input with noise */
                                frmevo, /* output which was corrected */
                                size, edge, edge);

                if (quality.fitness != fitness) {
                        fprintf(stderr, "Error in frame 0x%X: "
                                        "The computed fitness is %d (0x%X) "
                                        "and the parsed one is %d (0x%X)\n",
                                        vframe,
                                        quality.fitness, quality.fitness,
                                        fitness, fitness);
                        return EXIT_FAILURE;
                }
#if 0
                else
                        printf("Fitness for frame 0x%X successfully "
                                        "checked.\n", vframe);
#endif
        }

        xml_end_parse();

        fclose(vfile);

        deallocate_frm(frmin);
        deallocate_frm(frmdeterm);
        deallocate_frm(frmevo);
        deallocate_frm(frminclipped);
        deallocate_frm(frmdetclipped);

        printf("%d frames were checked and everything seem to be OK!\n",
                        vframe);

        return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
        int i;
        char *input = NULL;
        char *filterxml = NULL;
        int x = DEFAULT_X, y = DEFAULT_Y;
        int size = DEFAULT_SIZE;

        for (i = 1; i < argc; ++i) {
                if ((strcmp(argv[i], "-i") == 0) && ((i+1) < argc)) {
                        input = argv[++i];
                } else if (strcmp(argv[i], "-med") == 0) {
                        outop = MEDIAN;
                } else if ((strcmp(argv[i], "-x") == 0) && ((i+1) < argc)) {
                        x = (int) strtol(argv[++i], NULL, 10);
                } else if ((strcmp(argv[i], "-y") == 0) && ((i+1) < argc)) {
                        y = (int) strtol(argv[++i], NULL, 10);
                } else if ((strcmp(argv[i], "-f") == 0) && ((i+1) < argc)) {
                        filterxml = argv[++i];
                } else if ((strcmp(argv[i], "-s") == 0) && ((i+1) < argc)) {
                        size = (int) strtol(argv[++i], NULL, 10);
                } else if (strcmp(argv[i], "-edge") == 0) {
                        outop = EDGE;
                }
        }

        if ((input != NULL) && (filterxml != NULL) && (size > 0) &&
                        (outop != VOID) && x > 0 && y > 0)
                return process(input, filterxml, size, x, y);

        if (outop == VOID)
                fprintf(stderr, "The check module needs an operator for "
                                "image processing!\n");

        print_help(argv[0]);

        return EXIT_FAILURE;
}
