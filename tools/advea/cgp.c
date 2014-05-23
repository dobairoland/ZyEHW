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

#define IMG_T_MSB 0x80

cgp_t *cgp_allocate()
{
        cgp_t *cgp = malloc(sizeof(cgp_t));

        if (!cgp)
                fprintf(stderr, "CGP allocation error!");

        return cgp;
}

void cgp_deallocate(cgp_t *cgp)
{
        if (cgp != NULL)
                free(cgp);
}

static inline int is_bit_set64(uint32_t more_sig, uint32_t less_sig, int bit)
{
        /* if the selected bit is smaller than 32 then the selection is from
         * the lower half of the 64-word
         */

        return (bit < 32) ? ((less_sig & (1 << bit)) != 0)
                : ((more_sig & (1 << (bit - 32))) != 0);
}

static inline int is_bit_set(uint8_t i, int bit)
{
        return (i & (1 << bit)) != 0;
}

static int cgp_get_lut_input(img_t a, img_t b, int bit)
{
        int result = 0;
        const int a_i = is_bit_set(a, bit);
        const int a_i_plus = (bit == (CGP_BIT-1) ? is_bit_set(a, 0) :
                        is_bit_set(a, bit + 1));
        const int a_i_minus = (bit == 0 ? is_bit_set(a, CGP_BIT-1) :
                        is_bit_set(a, bit - 1));
        const int b_i = is_bit_set(b, bit);
        const int b_i_plus = (bit == (CGP_BIT-1) ? is_bit_set(b, 0) :
                        is_bit_set(b, bit + 1));
        const int b_i_minus = (bit == 0 ? is_bit_set(b, CGP_BIT-1) :
                        is_bit_set(b, bit - 1));

        /* the bits are connected in a circular way */

        /* LUT connections
         *  A0 => a(i-1),       if i = 0 then a(7)
         *  A1 => b(i-1),       if i = 0 then b(7)
         *  A2 => a(i),
         *  A3 => b(i),
         *  A4 => a(i+1),       if i = 7 then a(0)
         *  A5 => b(i+1)        if i = 7 then b(0)
         */

        if (a_i_minus)
                result |= (1 << 0);

        if (b_i_minus)
                result |= (1 << 1);

        if (a_i)
                result |= (1 << 2);

        if (b_i)
                result |= (1 << 3);

        if (a_i_plus)
                result |= (1 << 4);

        if (b_i_plus)
                result |= (1 << 5);

#if (CGP_BIT != 8)
#error CGP_BIT has been changed!
#endif

        return result;
}

static img_t cgp_evaluate_lut(const lut_t *msb, const lut_t *lsb,
                img_t a, img_t b)
{
        int i, select, bit, mask;
        img_t result = 0;

        for (i = 0, mask = 1; i < CGP_BIT; ++i, mask <<= 1) {
                select = cgp_get_lut_input(a, b, i);
                bit = is_bit_set64(msb[i], lsb[i], select);

                if (bit)
                        result |= mask;
        }

        return result;
}

static img_t cgp_get_response(const cgp_t *cgp, const img_t *pis)
{
        int i, j;
        img_t a, b, mux_a, mux_b;
        img_t reg[CGP_COL][CGP_ROW];

        for (i = 0; i < CGP_COL; ++i) {
                for (j = 0; j < CGP_ROW; ++j) {
                        mux_a = cgp->cgp_pe[i][j].mux_a;
                        mux_b = cgp->cgp_pe[i][j].mux_b;

                        mux_a = (i == 0 && mux_a >= CGP_PI) ? (mux_a - CGP_PI)
                                : mux_a;
                        mux_b = (i == 0 && mux_b >= CGP_PI) ? (mux_b - CGP_PI)
                                : mux_b;

                        a = mux_a < CGP_PI ? pis[mux_a] :
                                reg[i - 1][mux_a - CGP_PI];
                        b = mux_b < CGP_PI ? pis[mux_b] :
                                reg[i - 1][mux_b - CGP_PI];

                        reg[i][j] = cgp_evaluate_lut(cgp->cgp_pe[i][j].msb,
                                        cgp->cgp_pe[i][j].lsb, a, b);
                }
        }

        /*
        printf("sel: %d, func: %d\n", reg[CGP_COL-1][cgp->filter_switch],
                        reg[CGP_COL-1][cgp->out_select]);
                        */

        return ((reg[CGP_COL-1][cgp->filter_switch] & IMG_T_MSB) == IMG_T_MSB)
                ? reg[CGP_COL-1][cgp->out_select] : pis[4];
}

fitness_t cgp_compute_fitness(const cgp_t *cgp, const img_t *good_img,
                const img_t *bad_img, img_t *corrected_img, int img_size)
{
        img_t pis[CGP_PI];
        img_t response, good_pixel, abs_diff;
        int i;
        const int img_edge = sqrt(img_size);
        const int real_size = img_edge * img_edge;
        const int filtered_size = real_size - img_edge;
        fitness_t fit = 0;

        /* north border is not filtered */
        for (i = 0; i < img_edge; ++i)
                update_img(corrected_img, i, get_img(bad_img, i));

        /* south border is not filtered */
        for (i = filtered_size; i < real_size; ++i)
                update_img(corrected_img, i, get_img(bad_img, i));

        for (i = img_edge; i < filtered_size; ++i) {
                if (i % img_edge == 0) {
                        /* west border is not filtered */
                        update_img(corrected_img, i, get_img(bad_img, i));
                } else if (i % img_edge == (img_edge - 1)) {
                        /* east border is not filtered */
                        update_img(corrected_img, i, get_img(bad_img, i));
                } else {
                        pis[0] = get_img(bad_img, i - img_edge - 1);
                        pis[1] = get_img(bad_img, i - img_edge);
                        pis[2] = get_img(bad_img, i - img_edge + 1);
                        pis[3] = get_img(bad_img, i - 1);
                        pis[4] = get_img(bad_img, i);
                        pis[5] = get_img(bad_img, i + 1);
                        pis[6] = get_img(bad_img, i + img_edge - 1);
                        pis[7] = get_img(bad_img, i + img_edge);
                        pis[8] = get_img(bad_img, i + img_edge + 1);

                        /*
                        printf("%d %d %d %d [%d] %d %d %d %d\n", pis[0], pis[1],
                                        pis[2], pis[3], pis[4], pis[5], pis[6],
                                        pis[7], pis[8]);
                                        */

#if (CGP_PI != 9)
#error CGP_PI has been changed!
#endif

                        response = cgp_get_response(cgp, pis);
                        update_img(corrected_img, i, response);
                        good_pixel = get_img(good_img, i);
                        abs_diff = abs(((int)good_pixel) - ((int)response));
                        fit += abs_diff;

#if 0
                        printf("[good: %d, bad: %d], filter: %d, abs diff: %d, "
                                        "fitness: %d\n", good_pixel, pis[4],
                                        response, abs_diff, fit);
#endif
                }
        }

        return fit;
}

void cgp_initialize_pes(cgp_t *cgp)
{
        if (!cgp)
                return;

        memset(&cgp->cgp_pe, 0, sizeof(cgp->cgp_pe));
}

void cgp_print(const cgp_t *cgp)
{
        int i, j, k;

        if (!cgp)
                return;

        for (i = 0; i < CGP_COL; ++i) {
                for (j = 0; j < CGP_ROW; ++j) {

                        printf("col=%d, row=%d\n", i, j);

                        printf("\ta=0x%x, b=0x%x\n", cgp->cgp_pe[i][j].mux_a,
                                        cgp->cgp_pe[i][j].mux_b);

                        for (k = 0; k < CGP_BIT; ++k) {

                                printf("\t%d: 0x%.8x%.8x\n", k,
                                                cgp->cgp_pe[i][j].msb[k],
                                                cgp->cgp_pe[i][j].lsb[k]);
                        }
                }
        }

        printf("Filter switch: 0x%x\n", cgp->filter_switch);
        printf("Out select: 0x%x\n", cgp->out_select);
}
