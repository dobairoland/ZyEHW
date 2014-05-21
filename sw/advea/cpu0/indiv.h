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

#ifndef INDIV_H
#define INDIV_H

#include "xil_types.h"
#include "xtime_l.h"
#include "vrc.h"

#define CGP_PI          9
#define CGP_ROW         4
#define CGP_BIT         8
#define CGP_LUT_COL     2
#define CGP_LUTS_IN_COL (CGP_COL*CGP_ROW*CGP_BIT/CGP_LUT_COL)
/* +1 is the Hamming code */
#define CGP_LUT_WORDS   (CGP_LUTS_IN_COL/2 + 1)
#define LUT_T_BITS      32
#define LUT_BITS        64
#define BANKS           4
#define CGP_MUTATIONS   4

typedef short chrom_t;
typedef short func_t;
typedef u32 lut_t;

typedef struct {
        chrom_t mux_a;
        chrom_t mux_b;
        func_t f;
} pe_t;

typedef struct {
        pe_t pe_arr[CGP_COL][CGP_ROW];
        chrom_t filter_switch;
        chrom_t out_select;
        fitness_t fitness;
} cgp_indiv_t;

extern void init_indiv(cgp_indiv_t *indiv);
extern void copy_indiv(const cgp_indiv_t *src, cgp_indiv_t *dst);
extern int equal_indivs(const cgp_indiv_t *indiv1, const cgp_indiv_t *indiv2);
extern void indiv_to_fpga(cgp_indiv_t *indiv, int index);
extern void mutate_indiv(cgp_indiv_t *indiv);
extern void function_to_bitstream(int col, int row, func_t f);
extern void lut_from_bitstream(const cgp_indiv_t *indiv, int col, int row, int
                bit, lut_t *msb, lut_t *lsb);
extern XTime get_vrc_time();
extern void reset_vrc_time();
extern XTime get_dpr_time();
extern void reset_dpr_time();

#endif
