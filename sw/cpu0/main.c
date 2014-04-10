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

#include "xil_printf.h"
#include "xil_types.h"
#include "xstatus.h"
#include "xil_io.h"
#include "sd.h"
#include "ocm.h"
#include "cache.h"
#include <stdlib.h>
#include <limits.h>

#define FRAMESIZE       (FRAMEWIDTH*FRAMEHEIGHT)
#define TRAINEDGE       128
#define AXI0_BASE       0x43C00000
#define NUMBITSOF(a)    (CHAR_BIT*sizeof(a))

#define VIDEO3

#ifdef VIDEO1
/* Bunny video (240p resolution) with 5% salt-and-pepper noise */
#define XFILENAME       "h.dat"
#define FRAMECOUNT      14315
#endif

#ifdef VIDEO2
/* Foreman video with 5% salt-and-pepper noise */
#define XFILENAME       "i.dat"
#define FRAMECOUNT      300
#endif

#ifdef VIDEO3
/* Bunny video (240p resolution) with 5% salt-and-pepper noise */
/* First 3500 frames */
#define XFILENAME       "j.dat"
#define FRAMECOUNT      3500
#endif

#ifdef VIDEO2
#define FRAMEWIDTH      176
#define FRAMEHEIGHT     144
#else
#define FRAMEWIDTH      427
#define FRAMEHEIGHT     240
#endif

#if 0
#define EDGE_DETECTION
#endif

#ifdef EDGE_DETECTION
/* prefixing filename by "o" which indicates "original" video without noise */
#define FILENAME        "o" XFILENAME
#else
#define FILENAME        XFILENAME
#endif

static inline void order(u8 *a, u8 *b)
{
        if (*b < *a) {
                const u8 swap = *b;
                *b = *a;
                *a = swap;
        }
}

static inline u8 median(u8 k0, u8 k1, u8 k2, u8 k3, u8 k4, u8 k5, u8 k6,
                u8 k7, u8 k8)
{
        /* Paeth's 9-input median sorting network */

        order(&k0, &k3);
        order(&k1, &k4);
        order(&k0, &k1);
        order(&k2, &k5);
        order(&k0, &k2);
        order(&k4, &k5);
        order(&k1, &k2);
        order(&k3, &k5);
        order(&k3, &k4);
        order(&k1, &k3);
        order(&k1, &k6);
        order(&k4, &k6);
        order(&k2, &k6);
        order(&k2, &k3);
        order(&k4, &k7);
        order(&k2, &k4);
        order(&k3, &k7);
        order(&k4, &k8);
        order(&k3, &k8);
        order(&k3, &k4);

        return k4;
}

static inline long absolute_val(long op)
{
        return (op < 0) ? (-op) : op;
}

static inline u8 edge_operator(u8 k0, u8 k1, u8 k2, u8 k3, u8 k4, u8 k5,
                u8 k6, u8 k7, u8 k8)
{
        /* Sobel edge detector */

        const long p = k2 + 2*k5 + k8 - k0 - 2*k3 - k6;
        const long q = k6 + 2*k7 + k8 - k0 - 2*k1 - k2;

        (void) k4;

        return 128 + 1.0/8*absolute_val(p) + 1.0/8*absolute_val(q);
}

static inline void filter(const u8 *frmin, u8 *frmout)
{
        register u32 i, x, y;

        /* The borders are not filtered */
        for (i = 0; i < FRAMEWIDTH; ++i) {
#ifdef EDGE_DETECTION
                frmout[i] = /* upper border */
                frmout[(FRAMEHEIGHT-1)*FRAMEWIDTH+i] = /* bottom border */
                0; /* all will be black */
#else
                frmout[i] = frmin[i]; /* upper border */
                frmout[(FRAMEHEIGHT-1)*FRAMEWIDTH+i] = /* bottom border */
                        frmin[(FRAMEHEIGHT-1)*FRAMEWIDTH+i];
#endif
        }

        for (i = 0; i < FRAMEHEIGHT; ++i) {
#ifdef EDGE_DETECTION
                frmout[i*FRAMEWIDTH] = /* left border */
                frmout[(i+1)*FRAMEWIDTH-1] = /* right border */
                0; /* all will be black */
#else
                frmout[i*FRAMEWIDTH] = frmin[i*FRAMEWIDTH]; /* left border */
                frmout[(i+1)*FRAMEWIDTH-1] =
                        frmin[(i+1)*FRAMEWIDTH-1]; /* right border */
#endif
        }


        i = FRAMEWIDTH + 1; /* first row without north neighbor is
                            *  skipped; and skipping the border in the
                            *  next line */

        /* 1..FRAMEWIDTH-2 - because skipping the borders */
        for (y = 1; y < FRAMEHEIGHT-1; ++y) {
                for (x = 1; x < FRAMEWIDTH-1; ++x) {
                        frmout[i] =
#ifdef EDGE_DETECTION
                                edge_operator(
#else
                                median(
#endif
                                        frmin[i - FRAMEWIDTH - 1],
                                        frmin[i - FRAMEWIDTH],
                                        frmin[i - FRAMEWIDTH + 1],
                                        frmin[i - 1],
                                        frmin[i],
                                        frmin[i + 1],
                                        frmin[i + FRAMEWIDTH - 1],
                                        frmin[i + FRAMEWIDTH],
                                        frmin[i + FRAMEWIDTH + 1]);
                        ++i;
                }

                ++i; ++i;       /* two border pixels are skipped */
        }
}

static inline void send_training_set()
{
        register u32 i, j;
        static u8 frmin[FRAMESIZE];
        static u8 frmout[FRAMESIZE];

        if (!open_sd_file(FILENAME))
                return;

        for (i = 0; read_sd_file(frmin, FRAMESIZE) == FRAMESIZE; ++i) {

                filter(frmin, frmout);

                for (j = 0; j < FRAMESIZE; ++j) {
                        const div_t q = div(j, FRAMEWIDTH);
                        volatile u32 train_item =
                                (frmout[j] << NUMBITSOF(frmout[0]))
                                |
                                frmin[j];

                        if ((q.quot < (FRAMEHEIGHT-TRAINEDGE)/2) ||
                        (q.quot >= (FRAMEHEIGHT-TRAINEDGE)/2+TRAINEDGE) ||
                        (q.rem < (FRAMEWIDTH-TRAINEDGE)/2) ||
                        (q.rem >= (FRAMEWIDTH-TRAINEDGE)/2+TRAINEDGE))
                                continue;

                        /* Send only the middle part of the frame */

                        /* This read returns 1 if the fifo is near full (
                         * there is space for less than 100 items) */
                        while (Xil_In32(AXI0_BASE));

                        Xil_Out32(AXI0_BASE, train_item);
                }
        }

        close_sd_file();

        if (i != FRAMECOUNT)
                xil_printf("Error: %d frames were read instead of %d!\n\r", i,
                                FRAMECOUNT);

        OCM_FRAME_COUNTER = i;
}

static inline void setup_cache()
{
        /* The communication with the SD card does not allow to have cache
         * enabled. The OCM memory similarly shouldn't be cached.*/

        Ddisable();
}

static inline void execute()
{
        int i;

        OCM_CPU0_RUNNING = 1;
        OCM_CPU1_RUNNING = 0;

        while (!OCM_CPU1_RUNNING);

        /* The cache setup need to be run again because the boot script of the
         * other CPU changed it*/
        setup_cache();

        /* From now this CPU can use UART for errors only. */

        for (i = 0; i < RUNS; ++i) {

                OCM_CPU0_RUNNING = 1;
                send_training_set();
                OCM_CPU0_RUNNING = 0;

                while (OCM_CPU1_RUNNING);
        }
}

int main()
{
        xil_printf("CPU 0 begins...\n\r");

        setup_cache();
        execute();

        return XST_SUCCESS;
}
