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

#include "vrc.h"

#include "xil_types.h"
#include "xil_io.h"

#define AXI1_BASE               0x83C00000
#define AXI_ADDR_INC            0x4     /* 32-bit words are 4 bytes */

#define STAT_REG                0
#define STAT_START_MASK         (1 << 0)
#define STAT_END_MASK           (1 << 1)
#define STAT_RDERR_MASK         (1 << 2)
#define STAT_WRERR_MASK         (1 << 3)
#define STAT_FRAME_MASK         ((~((u32) 0)) ^ \
                                        (STAT_START_MASK | \
                                        STAT_END_MASK | \
                                        STAT_RDERR_MASK | \
                                        STAT_WRERR_MASK))
#define STAT_FRAME_SHIFT        4

#define MUX_BITS                4
#define MUX_MASK                0xF     /* four bits */

/* Bits of a column:
 * 3..0         row 0   input A
 * 7..4         row 0   input B
 * 11..8        row 1   input A
 * 15..12       row 1   input B
 * 19..16       row 2   input A
 * 23..20       row 2   input B
 * 27..24       row 3   input A
 * 31..28       row 3   input B
 */

/* Output select register:
 * bits 1..0 is out_select, bits 3..2 is filter_switch */
#define OUT_REG_MASK            0x3     /* two bits */
#define OUT_REG_SHIFT           2

#if (AXI_ADDR_INC == 4)
#define AXI_ADDR_INC_SHIFT      2
#endif

static inline u32 get_mux_column_reg(int indiv, int column)
{
        return AXI1_BASE + ((1 + /* 1 is the status register */
                indiv * (CGP_COL + 1) + /* +1 is the output register */
                column) << AXI_ADDR_INC_SHIFT);
}

static inline u32 get_out_reg(int indiv)
{
        return AXI1_BASE + ((1 + /* 1 is the status register */
                indiv * (CGP_COL + 1) + /* +1 is the output register */
                CGP_COL) << AXI_ADDR_INC_SHIFT);
}

static inline u32 get_fitness_reg(int indiv)
{
        return AXI1_BASE + ((1 + /* 1 is the status register */
                CGP_LAMBDA * (CGP_COL + 1) + /* +1 is the output register */
                indiv) << AXI_ADDR_INC_SHIFT);
}

static inline u32 read_status_reg()
{
        return Xil_In32(AXI1_BASE + STAT_REG);
}

int fifo_read_error()
{
        return ((read_status_reg() & STAT_RDERR_MASK) == STAT_RDERR_MASK);
}

int fifo_write_error()
{
        return ((read_status_reg() & STAT_WRERR_MASK) == STAT_WRERR_MASK);
}

void start_cgp()
{
        Xil_Out32(AXI1_BASE + STAT_REG, STAT_START_MASK);
}

void wait_cgp(fitness_t *fitarr, u32 *frames)
{
        int i;
        u32 stat_reg;

        do {
                stat_reg = read_status_reg();
        } while ((stat_reg & STAT_END_MASK) != STAT_END_MASK);

        *frames = (stat_reg & STAT_FRAME_MASK) >> STAT_FRAME_SHIFT;

        for (i = 0; i < CGP_LAMBDA; ++i)
                fitarr[i] = Xil_In32(get_fitness_reg(i));
}

u32 read_frame_number()
{
        return (read_status_reg() & STAT_FRAME_MASK) >> STAT_FRAME_SHIFT;
}

void send_vrc_column(int indiv, int column, u32 value)
{
        Xil_Out32(get_mux_column_reg(indiv, column), value);
}

void send_vrc_switch(int indiv, u32 value)
{
        Xil_Out32(get_out_reg(indiv), value);
}

u32 merge_vrc_switch(u8 out_select, u8 filter_switch)
{
        return ((filter_switch & OUT_REG_MASK) << OUT_REG_SHIFT) |
                (out_select & OUT_REG_MASK);
}

u32 merge_vrc_mux(u32 chrom, int row, u8 mux_a, u8 mux_b)
{
        return chrom |
                ((mux_b & MUX_MASK) << (2*MUX_BITS*row+MUX_BITS)) |
                ((mux_a & MUX_MASK) << (2*MUX_BITS*row));
}
