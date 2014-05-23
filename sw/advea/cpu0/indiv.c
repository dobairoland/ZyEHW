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

#include "indiv.h"

#include <stdlib.h>
#include <string.h>
#include "dpr.h"
#include "bitstream.h"

#if (CGP_ROW == 4)
#define CGP_ROW_MASK    0x3
#endif

#if 0
#define XML_PRINT_DEBUG
#include "xil_printf.h"
#endif

#define WORD_PER_FUNCTION       (CGP_BIT/2)
#define FUNC_NUM                (sizeof(pregen_func)/sizeof(pregen_func[0]))

#if (WORD_PER_FUNCTION == 4)
#define WORD_PER_FUNCTION_SHIFT   2
#endif

typedef u32 bank_func_t[WORD_PER_FUNCTION];
typedef bank_func_t pregen_func_t[BANKS];

static const pregen_func_t pregen_func[] = {
        { /* Function f(a, b) = 255 */
                { /* bank0 */
                        0xFFFFFFFF,
                        0xFFFFFFFF,
                        0xFFFFFFFF,
                        0xFFFFFFFF
                },
                { /* bank1 */
                        0xFFFFFFFF,
                        0xFFFFFFFF,
                        0xFFFFFFFF,
                        0xFFFFFFFF
                },
                { /* bank2 */
                        0xFFFFFFFF,
                        0xFFFFFFFF,
                        0xFFFFFFFF,
                        0xFFFFFFFF
                },
                { /* bank3 */
                        0xFFFFFFFF,
                        0xFFFFFFFF,
                        0xFFFFFFFF,
                        0xFFFFFFFF
                }
        },

        { /* Function f(a, b) = a */
                { /* bank0 */
                        0x33333333,
                        0x33333333,
                        0x33333333,
                        0x33333333
                },
                { /* bank1 */
                        0x33333333,
                        0x33333333,
                        0x33333333,
                        0x33333333
                },
                { /* bank2 */
                        0x33333333,
                        0x33333333,
                        0x33333333,
                        0x33333333
                },
                { /* bank3 */
                        0x33333333,
                        0x33333333,
                        0x33333333,
                        0x33333333
                }
        },

        { /* Function f(a, b) = 255 - a */
                { /* bank0 */
                        0xCCCCCCCC,
                        0xCCCCCCCC,
                        0xCCCCCCCC,
                        0xCCCCCCCC
                },
                { /* bank1 */
                        0xCCCCCCCC,
                        0xCCCCCCCC,
                        0xCCCCCCCC,
                        0xCCCCCCCC
                },
                { /* bank2 */
                        0xCCCCCCCC,
                        0xCCCCCCCC,
                        0xCCCCCCCC,
                        0xCCCCCCCC
                },
                { /* bank3 */
                        0xCCCCCCCC,
                        0xCCCCCCCC,
                        0xCCCCCCCC,
                        0xCCCCCCCC
                }
        },

        { /* Function f(a, b) = a >> 1 */
                { /* bank0 */
                        0x0F0F0F0F,
                        0x0F0F0F0F,
                        0x0F0F0F0F,
                        0x00000F0F
                },
                { /* bank1 */
                        0x0F0F0F0F,
                        0x0F0F0F0F,
                        0x0F0F0F0F,
                        0x00000F0F
                },
                { /* bank2 */
                        0x0F0F0F0F,
                        0x0F0F0F0F,
                        0x0F0F0F0F,
                        0x00000F0F
                },
                { /* bank3 */
                        0x0F0F0F0F,
                        0x0F0F0F0F,
                        0x0F0F0F0F,
                        0x00000F0F
                }
        },

        { /* Function f(a, b) = (a + b) >> 1; Fitness: 513968 */
                /* 0: 0xF044FFFF, 0xFFFFF040, */
                /* 1: 0xF8800FFF, 0x0FFFF880, */
                /* 2: 0xF8800FFF, 0x0FFFF880, */
                /* 3: 0xF8800FFF, 0x0FFFF880, */
                /* 4: 0xF8800FFF, 0x0FFFF880, */
                /* 5: 0xF8800FFF, 0x0FFFF880, */
                /* 6: 0xF8800FFF, 0x0FFFF880, */
                /* 7: 0xF880F880, 0xF880F880, */

                { /* bank0 */
                        0x0FF01FF5,
                        0x0FF00FF0,
                        0x0FF00FF0,
                        0x00000FF0
                },
                { /* bank1 */
                        0x1FF10FF0,
                        0x1FF11FF1,
                        0x1FF11FF1,
                        0x11111FF1
                },
                { /* bank2 */
                        0x7CC73FF3,
                        0x7CC77CC7,
                        0x7CC77CC7,
                        0x77777CC7
                },
                { /* bank3 */
                        0x3CC33FF3,
                        0x3CC33CC3,
                        0x3CC33CC3,
                        0x33333CC3
                }
        },

        { /* Function f(a, b) = max(a, b); Fitness: 531098 */
                /* 0: 0xD1008000, 0x0000D000, */
                /* 1: 0xFFF0FF00, 0xF0F0FFF0, */
                /* 2: 0xFFF0FF00, 0xF0F0FFF0, */
                /* 3: 0xFFF0FF00, 0xF0F0FFF0, */
                /* 4: 0xFFF0FF00, 0xF0F0FFF0, */
                /* 5: 0xFFF0FF00, 0xF0F0FFF0, */
                /* 6: 0xFFF0FF00, 0xF0F0FFF0, */
                /* 7: 0xFFF0FFF0, 0xFFF0FFF0, */

                { /* bank0 */
                        0x33030000,
                        0x33033303,
                        0x33033303,
                        0x33333303
                },
                { /* bank1 */
                        0x33030000,
                        0x33033303,
                        0x33033303,
                        0x33333303
                },
                { /* bank2 */
                        0xF3FF1011,
                        0xF3FFF3FF,
                        0xF3FFF3FF,
                        0xFFFFF3FF
                },
                { /* bank3 */
                        0xF3FF300B,
                        0xF3FFF3FF,
                        0xF3FFF3FF,
                        0xFFFFF3FF
                }
        },

        { /* Function f(a, b) = min(a, b); Fitness: 531174 */
                /* 0: 0xFFF6FFF6, 0xFFC7FFF4, */
                /* 1: 0xF000F0F0, 0xFF00F000, */
                /* 2: 0xF000F0F0, 0xFF00F000, */
                /* 3: 0xF000F0F0, 0xFF00F000, */
                /* 4: 0xF000F0F0, 0xFF00F000, */
                /* 5: 0xF000F0F0, 0xFF00F000, */
                /* 6: 0xF000F0F0, 0xFF00F000, */
                /* 7: 0xF000F000, 0xF000F000, */

                { /* bank0 */
                        0x00307D77,
                        0x00300030,
                        0x00300030,
                        0x00000030
                },
                { /* bank1 */
                        0x003039BB,
                        0x00300030,
                        0x00300030,
                        0x00000030
                },
                { /* bank2 */
                        0x3F33FFFF,
                        0x3F333F33,
                        0x3F333F33,
                        0x33333F33
                },
                { /* bank3 */
                        0x3F33FFFF,
                        0x3F333F33,
                        0x3F333F33,
                        0x33333F33
                }
        },

        { /* Function f(a, b) = a >> 2; Fitness: 1153024 */
                /* 0: 0xC8898888, 0x8DCC8C88, */
                /* 1: 0x70000000, 0xFF2F0000, */
                /* 2: 0x70000000, 0xFF2F0000, */
                /* 3: 0x70000000, 0xFF2F0000, */
                /* 4: 0x70000000, 0xFF2F0000, */
                /* 5: 0x70000000, 0xFF2F0000, */
                /* 6: 0x70000000, 0xFF2F0000, */
                /* 7: 0x00000000, 0x00000000, */

                { /* bank0 */
                        0x0C000508,
                        0x0C000C00,
                        0x0C000C00,
                        0x00000C00
                },
                { /* bank1 */
                        0x0E005555,
                        0x0E000E00,
                        0x0E000E00,
                        0x00000E00
                },
                { /* bank2 */
                        0x0F025555,
                        0x0F020F02,
                        0x0F020F02,
                        0x00000F02
                },
                { /* bank3 */
                        0x0F034C01,
                        0x0F030F03,
                        0x0F030F03,
                        0x00000F03
                }
        },

        { /* Function f(a, b) = a + b; Fitness: 2198776 */
                /* 0: 0x0F08E3B0, 0x00000C00, */
                /* 1: 0xAF0AA77A, 0xA11AAF0A, */
                /* 2: 0xAF0AA77A, 0xA11AAF0A, */
                /* 3: 0xAF0AA77A, 0xA11AAF0A, */
                /* 4: 0xAF0AA77A, 0xA11AAF0A, */
                /* 5: 0xAF0AA77A, 0xA11AAF0A, */
                /* 6: 0xAF0AA77A, 0xA11AAF0A, */
                /* 7: 0xA11AA77A, 0xA77AA77A, */

                { /* bank0 */
                        0x02300020,
                        0x02300230,
                        0x02300230,
                        0x33320230
                },
                { /* bank1 */
                        0xCCEC0034,
                        0xCCECCCEC,
                        0xCCECCCEC,
                        0xEEECCCEC
                },
                { /* bank2 */
                        0xF3BF40BC,
                        0xF3BFF3BF,
                        0xF3BFF3BF,
                        0xBBB3F3BF
                },
                { /* bank3 */
                        0xC8CC409C,
                        0xC8CCC8CC,
                        0xC8CCC8CC,
                        0xCCC8C8CC
                }
        },

        { /* Function f(a, b) = a +s b; Fitness: 510000 */
                /* 0: 0xEFFEFFFC, 0xFFFCAFFE, */
                /* 1: 0xFFFFFFF8, 0xFFF88FF8, */
                /* 2: 0xFFFFFFF8, 0xFFF88FF8, */
                /* 3: 0xFFFFFFF8, 0xFFF88FF8, */
                /* 4: 0xFFFFFFF8, 0xFFF88FF8, */
                /* 5: 0xFFFFFFF8, 0xFFF88FF8, */
                /* 6: 0xFFFFFFF8, 0xFFF88FF8, */
                /* 7: 0xFFF8FFF8, 0xFFF8FFF8, */

                { /* bank0 */
                        0x333F7777,
                        0x333F333F,
                        0x333F333F,
                        0x3333333F
                },
                { /* bank1 */
                        0xDFFFFFFF,
                        0xDFFFDFFF,
                        0xDFFFDFFF,
                        0xFFFFDFFF
                },
                { /* bank2 */
                        0xDFFFFFFF,
                        0xDFFFDFFF,
                        0xDFFFDFFF,
                        0xFFFFDFFF
                },
                { /* bank3 */
                        0xCFFFCFFD,
                        0xCFFFCFFF,
                        0xCFFFCFFF,
                        0xFFFFCFFF
                }
        },

        { /* Function f(a, b) = a > 127 b : a; Fitness: 783496 */
                /* 0: 0xFB59FFFE, 0xFF55FB50, */
                /* 1: 0xFD00F0F0, 0xFF00F0B0, */
                /* 2: 0xFD00F0F0, 0xFF00F0B0, */
                /* 3: 0xFD00F0F0, 0xFF00F0B0, */
                /* 4: 0xFD00F0F0, 0xFF00F0B0, */
                /* 5: 0xFD00F0F0, 0xFF00F0B0, */
                /* 6: 0xFD00F0F0, 0xFF00F0B0, */
                /* 7: 0xF000F000, 0xF000F000, */
                { /* bank0 */
                        0x20303F7B,
                        0x20302030,
                        0x20302030,
                        0x00002030
                },
                { /* bank1 */
                        0x303000F4,
                        0x30303030,
                        0x30303030,
                        0x00003030
                },
                { /* bank2 */
                        0x3F37FFFF,
                        0x3F373F37,
                        0x3F373F37,
                        0x33333F37
                },
                { /* bank3 */
                        0x3F3FBFFB,
                        0x3F3F3F3F,
                        0x3F3F3F3F,
                        0x33333F3F
                }
        },

        { /* Function f(a, b) = |a-b|; Fitness: 774184 */
                /* 0: 0x00300590, 0x02200420, */
                /* 1: 0x0DB00DD0, 0x0BB00DB0, */
                /* 2: 0x0DB00DD0, 0x0BB00DB0, */
                /* 3: 0x0DB00DD0, 0x0BB00DB0, */
                /* 4: 0x0DB00DD0, 0x0BB00DB0, */
                /* 5: 0x0DB00DD0, 0x0BB00DB0, */
                /* 6: 0x0DB00DD0, 0x0BB00DB0, */
                /* 7: 0x0DB00DB0, 0x0DB00DB0, */
                { /* bank0 */
                        0x22320022,
                        0x22322232,
                        0x22322232,
                        0x22222232
                },
                { /* bank1 */
                        0x33132212,
                        0x33133313,
                        0x33133313,
                        0x33333313
                },
                { /* bank2 */
                        0x4C440800,
                        0x4C444C44,
                        0x4C444C44,
                        0x44444C44
                },
                { /* bank3 */
                        0xC8CC40C0,
                        0xC8CCC8CC,
                        0xC8CCC8CC,
                        0xCCCCC8CC
                }
        }
};

static u32 *left_lut_col[] = {
        LEFT_BANK0,
        LEFT_BANK1,
        LEFT_BANK2,
        LEFT_BANK3
};

static u32 *right_lut_col[] = {
        RIGHT_BANK0,
        RIGHT_BANK1,
        RIGHT_BANK2,
        RIGHT_BANK3,
};

static XTime vrc_time_acc = 0;
static XTime dpr_time_acc = 0;

static void update_bitstream(int col, int row, func_t f)
{
        /* PE[0][0] DLUT CLUT             PE[7][3] DLUT CLUT
         *          BLUT ALUT                      BLUT ALUT
         *          DLUT CLUT                      DLUT CLUT
         *          BLUT ALUT                      BLUT ALUT
         * PE[0][1] DLUT CLUT             PE[7][2] DLUT CLUT
         *          BLUT ALUT                      BLUT ALUT
         *          DLUT CLUT                      DLUT CLUT
         *          BLUT ALUT                      BLUT ALUT
         * ...                            ...
         *
         * PE[3][3] DLUT CLUT             PE[4][0] DLUT CLUT
         *          BLUT ALUT                      BLUT ALUT
         *          DLUT CLUT                      DLUT CLUT
         *          BLUT ALUT                      BLUT ALUT
         */

        const int right = (col >= (CGP_COL/CGP_LUT_COL));
        u32 **lut_col = right ? right_lut_col : left_lut_col;
        u32 *b0 = lut_col[0];
        u32 *b1 = lut_col[1];
        u32 *b2 = lut_col[2];
        u32 *b3 = lut_col[3];
        int offset = BANK_SIZE-WORD_PER_FUNCTION-1-
                ((right ? (CGP_COL-1-col)*CGP_ROW+(CGP_ROW-1-row) :
                        (col*CGP_ROW+row)) << WORD_PER_FUNCTION_SHIFT);
        int line0 = offset;
        int line1 = ++offset;
        int line2 = ++offset;
        int line3 = ++offset;

        if (line3 >= HAMMING_OFF) {
                if (line0 < HAMMING_OFF) {
                        /* only some of them are after the Hamming line */
                        if (line1 >= HAMMING_OFF) {
                                line1 = line2;
                                line2 = line3;
                                ++line3;
                        } else if (line2 >= HAMMING_OFF) {
                                line2 = line3;
                                ++line3;
                        } else {
                                /* only line3 >= HAMMING_OFF */
                                ++line3;
                        }
                } else {
                        /* they all are after the Hamming line */
                        line0 = line1;
                        line1 = line2;
                        line2 = line3;
                        ++line3;
                }
        }

        b0[line0] = pregen_func[f][0][0];
        b0[line1] = pregen_func[f][0][1];
        b0[line2] = pregen_func[f][0][2];
        b0[line3] = pregen_func[f][0][3];

        b1[line0] = pregen_func[f][1][0];
        b1[line1] = pregen_func[f][1][1];
        b1[line2] = pregen_func[f][1][2];
        b1[line3] = pregen_func[f][1][3];

        b2[line0] = pregen_func[f][2][0];
        b2[line1] = pregen_func[f][2][1];
        b2[line2] = pregen_func[f][2][2];
        b2[line3] = pregen_func[f][2][3];

        b3[line0] = pregen_func[f][3][0];
        b3[line1] = pregen_func[f][3][1];
        b3[line2] = pregen_func[f][3][2];
        b3[line3] = pregen_func[f][3][3];
}

static inline void mutate_connection(chrom_t *chrom)
{
        *chrom = rand() % (CGP_PI + CGP_ROW);
}

static inline void mutate_connection_out(chrom_t *chrom)
{
        *chrom = rand() & CGP_ROW_MASK;
}

static inline void mutate_function(func_t *f)
{
        *f = rand() % FUNC_NUM;
}

void init_indiv(cgp_indiv_t *indiv)
{
        int i, j;

        for (i = 0; i < CGP_COL; ++i) {
                for (j = 0; j < CGP_ROW; ++j) {
                        pe_t *pe = &(indiv->pe_arr[i][j]);
                        mutate_connection(&(pe->mux_a));
                        mutate_connection(&(pe->mux_b));
                        mutate_function(&(pe->f));
                }
        }

        mutate_connection_out(&(indiv->filter_switch));
        mutate_connection_out(&(indiv->out_select));
        indiv->fitness = ~((fitness_t) 0);
}

void indiv_to_fpga(cgp_indiv_t *indiv, int index)
{
        int i, j;
        u32 vrc_chrom;
        XTime start, end;

        /* VRC start */
        XTime_GetTime(&start);

        for (i = 0; i < CGP_COL; ++i) {
                vrc_chrom = 0;

                for (j = 0; j < CGP_ROW; ++j) {
                        const pe_t *pe = &indiv->pe_arr[i][j];
                        vrc_chrom = merge_vrc_mux(vrc_chrom, j, pe->mux_a,
                                        pe->mux_b);
                        update_bitstream(i, j, pe->f);
                }

                send_vrc_column(index, i, vrc_chrom);
        }

        send_vrc_switch(index, merge_vrc_switch(indiv->out_select,
                                indiv->filter_switch));
        XTime_GetTime(&end);
        vrc_time_acc += end - start;
        /* VRC end */

        /* DPR start */
        XTime_GetTime(&start);
        set_left_far(index);
        set_right_far(index);
        flush_bitstream();
        dpr_reconfigure();
        XTime_GetTime(&end);
        dpr_time_acc += end - start;
        /* DPR end */
}

void copy_indiv(const cgp_indiv_t *src, cgp_indiv_t *dst)
{
        memcpy(dst, src, sizeof(cgp_indiv_t));
}

void mutate_indiv(cgp_indiv_t *indiv)
{
        const int randcol = rand() % (CGP_COL + 1);

        if (randcol == CGP_COL) {
                mutate_connection_out((rand() & 1) ? &indiv->filter_switch :
                                &indiv->out_select);
        } else {
                const int randrow = rand() & CGP_ROW_MASK;
                const int randit = rand() % 3; /* PE contains 3 elements */

                pe_t *pe = &(indiv->pe_arr[randcol][randrow]);

                if (randit == 0) {
                        mutate_connection(&(pe->mux_a));
                } else if (randit == 1) {
                        mutate_connection(&(pe->mux_b));
                } else
                        mutate_function(&(pe->f));
        }
}

static lut_t extract_quarter_lut(lut_t b0, lut_t b1, lut_t b2, lut_t b3,
                lut_t lut_mask, lut_t retval_bit)
{
        int i;
        lut_t retval = 0;

        for (i = 0; i < LUT_T_BITS/2; i += BANKS, lut_mask <<= 1) {

                if (b2 & lut_mask)
                        retval |= retval_bit;

                retval_bit >>= 1;

                if (b3 & lut_mask)
                        retval |= retval_bit;

                /* 8 steps later is the same lut_mask */
                retval_bit >>= (8 - 1);

                if (b1 & lut_mask)
                        retval |= retval_bit;

                retval_bit >>= 1;

                if (b0 & lut_mask)
                        retval |= retval_bit;

                retval_bit <<= (8 - 1);
        }

        return retval;
}

static lut_t extract_half_lut(lut_t b0, lut_t b1, lut_t b2, lut_t b3,
                lut_t lut_mask)
{
        const lut_t retval_bit1 = (1 << (LUT_T_BITS - 1));
        const lut_t retval_bit2 = (1 << (LUT_T_BITS/2 - 1));
        const lut_t lut_mask2 = lut_mask << LUT_T_BITS/2/BANKS;
        lut_t msb = extract_quarter_lut(b0, b1, b2, b3, lut_mask, retval_bit1);
        lut_t lsb = extract_quarter_lut(b0, b1, b2, b3, lut_mask2, retval_bit2);

        /* lut_mask2 should continue where lut_mask ends */

        return msb | lsb;
}

void function_to_bitstream(int col, int row, func_t f)
{
        update_bitstream(col, row, f);
}

void lut_from_bitstream(const cgp_indiv_t *indiv, int col, int row, int bit,
                lut_t *msb, lut_t *lsb)
{
        /*
         * X__Y49 DLUT [0][0].7 OFFSET63.MSB            X__Y49 [3][7].7
         *        CLUT       .6         .LSB
         *        BLUT       .5 OFFSET62.MSB
         *        ALUT       .4         .LSB
         *    Y48 DLUT       .3 OFFSET61.MSB               Y48
         *        CLUT       .2         .LSB
         *        BLUT       .1 OFFSET60.MSB
         *        ALUT       .0         .LSB                   [3][7].0
         *
         * ...
         *
         *    Y19 DLUT [3][3].7  OFFSET3.MSB               Y19 [0][4].7
         *        CLUT       .6         .LSB
         *        BLUT       .5  OFFSET2.MSB
         *        ALUT       .4         .LSB
         *    Y18 DLUT       .3  OFFSET1.MSB
         *        CLUT       .2         .LSB
         *        BLUT       .1  OFFSET0.MSB
         *        ALUT       .0         .LSB                   [0][4].0
         */

        const lut_t lut_mask = (bit & 1) ? (1 << (LUT_T_BITS/2)) : 1;
        const int right = (col >= (CGP_COL/CGP_LUT_COL));
        u32 **lut_col = right ? right_lut_col : left_lut_col;
        u32 *bank0 = lut_col[0];
        u32 *bank1 = lut_col[1];
        u32 *bank2 = lut_col[2];
        u32 *bank3 = lut_col[3];
        int offset = BANK_SIZE-WORD_PER_FUNCTION-1-
                ((right ? (CGP_COL-1-col)*CGP_ROW+(CGP_ROW-1-row) :
                        (col*CGP_ROW+row)) << WORD_PER_FUNCTION_SHIFT);

        offset += bit/2;

        if (offset >= HAMMING_OFF)
                ++offset;

        /*
         *      bit63 bank2 XXXXXXXXXXXXXXX1
         *      bit62 bank3 XXXXXXXXXXXXXXX1
         *      bit61 bank2 XXXXXXXXXXXXXX1X
         *      bit60 bank3 XXXXXXXXXXXXXX1X
         *      bit59 bank2 XXXXXXXXXXXXX1XX
         *      bit58 bank3 XXXXXXXXXXXXX1XX
         *      bit57 bank2 XXXXXXXXXXXX1XXX
         *      bit56 bank3 XXXXXXXXXXXX1XXX
         *
         *      bit55 bank1 XXXXXXXXXXXXXXX1
         *      bit54 bank0 XXXXXXXXXXXXXXX1
         *      bit53 bank1 XXXXXXXXXXXXXX1X
         *      bit52 bank0 XXXXXXXXXXXXXX1X
         *      bit51 bank1 XXXXXXXXXXXXX1XX
         *      bit50 bank0 XXXXXXXXXXXXX1XX
         *      bit49 bank1 XXXXXXXXXXXX1XXX
         *      bit48 bank0 XXXXXXXXXXXX1XXX
         *
         *      bit47 bank2 XXXXXXXXXXX1XXXX
         *      bit46 bank3 XXXXXXXXXXX1XXXX
         *      bit45 bank2 XXXXXXXXXX1XXXXX
         *      bit44 bank3 XXXXXXXXXX1XXXXX
         *      bit43 bank2 XXXXXXXXX1XXXXXX
         *      bit42 bank3 XXXXXXXXX1XXXXXX
         *      bit41 bank2 XXXXXXXX1XXXXXXX
         *      bit40 bank3 XXXXXXXX1XXXXXXX
         *
         *      bit39 bank1 XXXXXXXXXXX1XXXX
         *      bit38 bank0 XXXXXXXXXXX1XXXX
         *      bit37 bank1 XXXXXXXXXX1XXXXX
         *      bit36 bank0 XXXXXXXXXX1XXXXX
         *      bit35 bank1 XXXXXXXXX1XXXXXX
         *      bit34 bank0 XXXXXXXXX1XXXXXX
         *      bit33 bank1 XXXXXXXX1XXXXXXX
         *      bit32 bank0 XXXXXXXX1XXXXXXX
         *
         *      bit31 bank2 XXXXXXX1XXXXXXXX
         *      bit30 bank3 XXXXXXX1XXXXXXXX
         *      bit29 bank2 XXXXXX1XXXXXXXXX
         *      bit28 bank3 XXXXXX1XXXXXXXXX
         *      bit27 bank2 XXXXX1XXXXXXXXXX
         *      bit26 bank3 XXXXX1XXXXXXXXXX
         *      bit25 bank2 XXXX1XXXXXXXXXXX
         *      bit24 bank3 XXXX1XXXXXXXXXXX
         *
         *      bit23 bank1 XXXXXXX1XXXXXXXX
         *      bit22 bank0 XXXXXXX1XXXXXXXX
         *      bit21 bank1 XXXXXX1XXXXXXXXX
         *      bit20 bank0 XXXXXX1XXXXXXXXX
         *      bit19 bank1 XXXXX1XXXXXXXXXX
         *      bit18 bank0 XXXXX1XXXXXXXXXX
         *      bit17 bank1 XXXX1XXXXXXXXXXX
         *      bit16 bank0 XXXX1XXXXXXXXXXX
         *
         *      bit15 bank2 XXX1XXXXXXXXXXXX
         *      bit14 bank3 XXX1XXXXXXXXXXXX
         *      bit13 bank2 XX1XXXXXXXXXXXXX
         *      bit12 bank3 XX1XXXXXXXXXXXXX
         *      bit11 bank2 X1XXXXXXXXXXXXXX
         *      bit10 bank3 X1XXXXXXXXXXXXXX
         *      bit 9 bank2 1XXXXXXXXXXXXXXX
         *      bit 8 bank3 1XXXXXXXXXXXXXXX
         *
         *      bit 7 bank1 XXX1XXXXXXXXXXXX
         *      bit 6 bank0 XXX1XXXXXXXXXXXX
         *      bit 5 bank1 XX1XXXXXXXXXXXXX
         *      bit 4 bank0 XX1XXXXXXXXXXXXX
         *      bit 3 bank1 X1XXXXXXXXXXXXXX
         *      bit 2 bank0 X1XXXXXXXXXXXXXX
         *      bit 1 bank1 1XXXXXXXXXXXXXXX
         *      bit 0 bank0 1XXXXXXXXXXXXXXX
         */

        *msb = extract_half_lut(bank0[offset], bank1[offset],
                                bank2[offset], bank3[offset],
                                lut_mask);
        *lsb = extract_half_lut(bank0[offset], bank1[offset],
                                bank2[offset], bank3[offset],
                                lut_mask << (LUT_T_BITS/2/2));

#ifdef XML_PRINT_DEBUG
         xil_printf("[%d][%d].%d:\tb0: 0x%x\n\r", col, row, bit,
                         bank0[offset]);
         xil_printf("\t\tb1: 0x%x\n\r", bank1[offset]);
         xil_printf("\t\tb2: 0x%x\n\r", bank2[offset]);
         xil_printf("\t\tb3: 0x%x\n\r", bank3[offset]);
         xil_printf("\t\toff %d\n\r", offset);
#endif
}

XTime get_vrc_time()
{
        return vrc_time_acc;
}

void reset_vrc_time()
{
        vrc_time_acc = 0;
}

XTime get_dpr_time()
{
        return dpr_time_acc;
}

void reset_dpr_time()
{
        dpr_time_acc = 0;
}
