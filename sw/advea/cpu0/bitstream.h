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

#ifndef BITSTREAM_H
#define BITSTREAM_H

#include "xil_types.h"

#define IND_HEADER_LINES         29
#define BANK_SIZE                101
#define LBANK0_OFF               IND_HEADER_LINES
#define LBANK1_OFF               (LBANK0_OFF + BANK_SIZE)
#define LBANK2_OFF               (LBANK1_OFF + BANK_SIZE)
#define LBANK3_OFF               (LBANK2_OFF + BANK_SIZE)
#define NEXTINDHEADLINES         7
#define RBANK0_OFF               (LBANK3_OFF + 2*BANK_SIZE + NEXTINDHEADLINES)
#define RBANK1_OFF               (RBANK0_OFF + BANK_SIZE)
#define RBANK2_OFF               (RBANK1_OFF + BANK_SIZE)
#define RBANK3_OFF               (RBANK2_OFF + BANK_SIZE)

#define LEFT_BANK0               &indiv_lut_stream[LBANK0_OFF]
#define LEFT_BANK1               &indiv_lut_stream[LBANK1_OFF]
#define LEFT_BANK2               &indiv_lut_stream[LBANK2_OFF]
#define LEFT_BANK3               &indiv_lut_stream[LBANK3_OFF]

#define RIGHT_BANK0              &indiv_lut_stream[RBANK0_OFF]
#define RIGHT_BANK1              &indiv_lut_stream[RBANK1_OFF]
#define RIGHT_BANK2              &indiv_lut_stream[RBANK2_OFF]
#define RIGHT_BANK3              &indiv_lut_stream[RBANK3_OFF]

#define POP_HEADER_LINES         29
#define NEXT_HEAD_LINES          7

#define IND0_OFF                 POP_HEADER_LINES
#define IND1_OFF                 (IND0_OFF + 2*BANK_SIZE + NEXT_HEAD_LINES)
#define IND2_OFF                 (IND1_OFF + 2*BANK_SIZE + NEXT_HEAD_LINES)
#define IND3_OFF                 (IND2_OFF + 2*BANK_SIZE + NEXT_HEAD_LINES)
#define IND4_OFF                 (IND3_OFF + 2*BANK_SIZE + NEXT_HEAD_LINES)
#define IND5_OFF                 (IND4_OFF + 2*BANK_SIZE + NEXT_HEAD_LINES)

#define IND0_BANK                &popul_mut_stream[IND0_OFF]
#define IND1_BANK                &popul_mut_stream[IND1_OFF]
#define IND2_BANK                &popul_mut_stream[IND2_OFF]
#define IND3_BANK                &popul_mut_stream[IND3_OFF]
#define IND4_BANK                &popul_mut_stream[IND4_OFF]
#define IND5_BANK                &popul_mut_stream[IND5_OFF]

#define HAMMING_OFF              50

extern u32 indiv_lut_stream[];
extern u32 popul_mut_stream[];

extern int size_of_indiv_stream();
extern void set_left_far(int individual);
extern void set_right_far(int individual);
extern void flush_indiv_stream();

extern int size_of_popul_stream();
extern void flush_popul_stream();
extern void set_pop_far(int individual, int lut, int right);

#endif
