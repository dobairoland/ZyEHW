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

#ifndef VRC_H
#define VRC_H

#include "xil_types.h"

#define CGP_COL                 8
#define CGP_INDIVS              6

typedef u32 fitness_t;

extern int fifo_read_error();
extern int fifo_write_error();
extern void start_cgp();
extern void wait_cgp(fitness_t *fitarr, u32 *frames);
extern u32 read_frame_number();
extern void send_vrc_column(int indiv, int column, u32 value);
extern void send_vrc_switch(int indiv, u32 value);
extern u32 merge_vrc_switch(u8 out_select, u8 filter_switch);
extern u32 merge_vrc_mux(u32 chrom, int row, u8 mux_a, u8 mux_b);

#endif
