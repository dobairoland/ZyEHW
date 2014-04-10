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

#ifndef CGP_H
#define CGP_H

#include <inttypes.h>
#include "frm.h"

#define CGP_PI          9
#define CGP_BIT         8
#define CGP_COL         8
#define CGP_ROW         4

typedef uint32_t fitness_t;
typedef double psnr_t;
typedef uint8_t mux_t;
typedef uint32_t lut_t;

typedef struct {
        mux_t mux_a;
        mux_t mux_b;
        lut_t msb[CGP_BIT];
        lut_t lsb[CGP_BIT];
} cgp_pe_t;

typedef struct {
        cgp_pe_t cgp_pe[CGP_COL][CGP_ROW];
        mux_t filter_switch;
        mux_t out_select;
} cgp_t;

typedef struct {
        fitness_t fitness;
        psnr_t psnr_bad;
        psnr_t psnr_filtered;
} quality_t;

extern cgp_t *cgp_allocate();
extern void cgp_deallocate(cgp_t *cgp);
extern quality_t cgp_compute_fitness(const cgp_t *cgp, const frm_t *good_img,
                const frm_t *bad_img, frm_t *corrected_img, int img_size,
                int x, int y);
extern void cgp_initialize_pes(cgp_t *cgp);
extern void cgp_print(const cgp_t *cgp);

#endif
