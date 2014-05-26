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

#include "bitstream.h"

#include "xil_cache.h"

#define EMPTY_BANK      0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000, \
                        0x00000000

#define SLICE_X1Y0_X1Y49        0x0042011A /* bott, row 1, col  2, minor 26 */
#define SLICE_X11Y0_X11Y49      0x0042041A /* bott, row 1, col  8, minor 26 */
#define SLICE_X33Y0_X33Y49      0x00420B9A /* bott, row 1, col 23, minor 26 */
#define SLICE_X43Y0_X43Y49      0x00420E9A /* bott, row 1, col 29, minor 26 */
#define SLICE_X65Y0_X65Y49      0x0042151A /* bott, row 1, col 42, minor 26 */
#define SLICE_X75Y0_X75Y49      0x0042179A /* bott, row 1, col 47, minor 26 */
#define SLICE_X97Y0_X97Y49      0x00421E9A /* bott, row 1, col 61, minor 26 */
#define SLICE_X107Y0_X107Y49    0x0042221A /* bott, row 1, col 68, minor 26 */
#define SLICE_X97Y50_X97Y99     0x00401E9A /* bott, row 0, col 61, minor 26 */
#define SLICE_X107Y50_X107Y99   0x0040221A /* bott, row 0, col 68, minor 26 */
#define SLICE_X97Y100_X97Y149   0x00001E9A /*  top, row 0, col 61, minor 26 */
#define SLICE_X107Y100_X107Y149 0x0000221A /*  top, row 0, col 68, minor 26 */

#define MINOR_ADDR_MASK         0xFFFFFF80

#define IND_FAR1_OFF            26
#define IND_FAR2_OFF            (IND_HEADER_LINES + 5*BANK_SIZE + 4)

#define POP_FAR1_OFF            30
#define POP_FAR2_OFF            (IND0_OFF + BANK_SIZE + 4)
#define POP_FAR3_OFF            (IND1_OFF + BANK_SIZE + 4)
#define POP_FAR4_OFF            (IND2_OFF + BANK_SIZE + 4)
#define POP_FAR5_OFF            (IND3_OFF + BANK_SIZE + 4)
#define POP_FAR6_OFF            (IND4_OFF + BANK_SIZE + 4)

static u32 left_fars[] = {
        SLICE_X1Y0_X1Y49,
        SLICE_X33Y0_X33Y49,
        SLICE_X65Y0_X65Y49,
        SLICE_X97Y0_X97Y49,
        SLICE_X97Y50_X97Y99,
        SLICE_X97Y100_X97Y149
};

static u32 right_fars[] = {
        SLICE_X11Y0_X11Y49,
        SLICE_X43Y0_X43Y49,
        SLICE_X75Y0_X75Y49,
        SLICE_X107Y0_X107Y49,
        SLICE_X107Y50_X107Y99,
        SLICE_X107Y100_X107Y149
};

static u32 pop_fars[] = {
        POP_FAR1_OFF,
        POP_FAR2_OFF,
        POP_FAR3_OFF,
        POP_FAR4_OFF,
        POP_FAR5_OFF,
        POP_FAR6_OFF
};

static u8 minor_addr[] = {
        26,
        27,
        28,
        29
};

u32 indiv_lut_stream[] = {
        0xFFFFFFFF, /* Bus width auto detection */
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF, /* Bus width auto detection ends */
        0x000000BB, /* Bus width pattern 1*/
        0x11220044, /* Bus width pattern 2 */
        0xFFFFFFFF, /* Bus width auto detection */
        0xFFFFFFFF, /* Bus width auto detection */
        0xAA995566, /* Sync word */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000007, /* Resets the CRC register. */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30018001, /* Type 1 packet, Write, Device ID Register */
        0x03727093,
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000000, /* Null command.*/
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000001, /* Writes configuration data */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30002001, /* Type 1 packet, Write, Frame Address Register */
        0x00000000, /* Frame address */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x300041F9, /* Type 1 packet, Write, Frame Data Input Register */
        EMPTY_BANK, /* Bank 0 */                        /* 5*101 = 505 */
        EMPTY_BANK, /* Bank 1 */
        EMPTY_BANK, /* Bank 2 */
        EMPTY_BANK, /* Bank 3 */
        EMPTY_BANK, /* An empty, not used should be at the end */
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000001, /* Writes configuration data */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30002001, /* Type 1 packet, Write, Frame Address Register */
        0x00000000, /* Frame address */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x300041F9, /* Type 1 packet, Write, Frame Data Input Register */
        EMPTY_BANK, /* Bank 0 */                        /* 5*101 = 505 */
        EMPTY_BANK, /* Bank 1 */
        EMPTY_BANK, /* Bank 2 */
        EMPTY_BANK, /* Bank 3 */
        EMPTY_BANK, /* An empty, not used should be at the end */
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000003, /* Last frame: GHIGH_B, interconnects. */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30002001, /* Type 1 packet, Write, Frame Address Register */
        0x03BE0000, /* Dummy frame address */
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000007, /* Resets the CRC register. */
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x0000000D, /* Resets the DALIGN signal */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000  /* Type 1 packet, NOP, Word count 0 */
};

u32 popul_mut_stream[] = {
        0xFFFFFFFF, /* Bus width auto detection */
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF, /* Bus width auto detection ends */
        0x000000BB, /* Bus width pattern 1*/
        0x11220044, /* Bus width pattern 2 */
        0xFFFFFFFF, /* Bus width auto detection */
        0xFFFFFFFF, /* Bus width auto detection */
        0xAA995566, /* Sync word */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000007, /* Resets the CRC register. */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30018001, /* Type 1 packet, Write, Device ID Register */
        0x03727093,
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000000, /* Null command.*/
        0x3000C001, /* Type 1 packet, Write, Masking register */
        0x00200000, /* CTL1 */
        0x30030001, /* Type 1 packet, Write, Control Register 1 */
        0x00200000, /* reserved according to the documentation */
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000001, /* Writes configuration data */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30002001, /* Type 1 packet, Write, Frame Address Register */
        0x00000000, /* Frame address */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30004065, /* Type 1 packet, Write, Frame Data Input Register */
        EMPTY_BANK,
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000001, /* Writes configuration data */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30002001, /* Type 1 packet, Write, Frame Address Register */
        0x00000000, /* Frame address */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30004065, /* Type 1 packet, Write, Frame Data Input Register */
        EMPTY_BANK,
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000001, /* Writes configuration data */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30002001, /* Type 1 packet, Write, Frame Address Register */
        0x00000000, /* Frame address */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30004065, /* Type 1 packet, Write, Frame Data Input Register */
        EMPTY_BANK,
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000001, /* Writes configuration data */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30002001, /* Type 1 packet, Write, Frame Address Register */
        0x00000000, /* Frame address */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30004065, /* Type 1 packet, Write, Frame Data Input Register */
        EMPTY_BANK,
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000001, /* Writes configuration data */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30002001, /* Type 1 packet, Write, Frame Address Register */
        0x00000000, /* Frame address */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30004065, /* Type 1 packet, Write, Frame Data Input Register */
        EMPTY_BANK,
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000001, /* Writes configuration data */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30002001, /* Type 1 packet, Write, Frame Address Register */
        0x00000000, /* Frame address */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30004065, /* Type 1 packet, Write, Frame Data Input Register */
        EMPTY_BANK,
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000000, /* Null command */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x3000C001, /* Type 1 packet, Write, Masking register */
        0x00200000, /* CTL1 */
        0x30030001, /* Type 1 packet, Write, Control Register 1 */
        0x00000000, /* reserved according to the documentation */
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000003, /* Last frame: GHIGH_B, interconnects. */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x30002001, /* Type 1 packet, Write, Frame Address Register */
        0x03BE0000, /* Dummy frame address */
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x00000007, /* Resets the CRC register. */
        0x30008001, /* Type 1 packet, Write, Command Register */
        0x0000000D, /* Resets the DALIGN signal */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000, /* Type 1 packet, NOP, Word count 0 */
        0x20000000  /* Type 1 packet, NOP, Word count 0 */
};

int size_of_indiv_stream()
{
        return sizeof(indiv_lut_stream)/sizeof(indiv_lut_stream[0]);
}

void set_left_far(int individual)
{
         indiv_lut_stream[IND_FAR1_OFF] = left_fars[individual];
}

void set_right_far(int individual)
{
        indiv_lut_stream[IND_FAR2_OFF] = right_fars[individual];
}

void flush_indiv_stream()
{
        Xil_DCacheFlushRange((u32) indiv_lut_stream, sizeof(indiv_lut_stream));
}

int size_of_popul_stream()
{
        return sizeof(popul_mut_stream)/sizeof(popul_mut_stream[0]);
}

void flush_popul_stream()
{
        Xil_DCacheFlushRange((u32) popul_mut_stream, sizeof(popul_mut_stream));
}

void set_pop_far(int individual, int lut, int right)
{
        u32 frame_address = right ? right_fars[individual] :
                left_fars[individual];

        frame_address = (frame_address & MINOR_ADDR_MASK) | minor_addr[lut];

        popul_mut_stream[pop_fars[individual]] = frame_address;
}
