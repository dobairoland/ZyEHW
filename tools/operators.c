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

#include <stdlib.h>
#include <math.h>

#define BLACK   0

#include "operators.h"

typedef frm_t (*operator_callback)(frm_t k0, frm_t k1, frm_t k2, frm_t k3, frm_t
                k4, frm_t k5, frm_t k6, frm_t k7, frm_t k8);

void apply_void(frm_t *frm, int frmsize)
{
        int i;

        for (i = 0; i < frmsize; ++i)
                update_frm(frm, i, rand() & (~((frm_t) 0)));
}

static inline void order(frm_t *a, frm_t *b)
{
        if (*b < *a) {
                const frm_t swap = *b;
                *b = *a;
                *a = swap;
        }
}

static inline frm_t paeth_median(frm_t k0, frm_t k1, frm_t k2, frm_t k3, frm_t
                k4, frm_t k5, frm_t k6, frm_t k7, frm_t k8)
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

static void apply_operator(int sx, int sy, const frm_t *frmin, frm_t *frmout,
                operator_callback callback)
{
        const int xend = sx - 1, yend = sy - 1;
        int x, y;
        int i = sx + 1; /* first row without north neighbor is skipped;
                               skipping the border */

        for (y = 1; y < yend; ++y) {
                for (x = 1; x < xend; ++x) {
                        update_frm(frmout, i, (*callback)(
                                get_frm(frmin, i - sx - 1),
                                get_frm(frmin, i - sx),
                                get_frm(frmin, i - sx + 1),
                                get_frm(frmin, i - 1),
                                get_frm(frmin, i),
                                get_frm(frmin, i + 1),
                                get_frm(frmin, i + sx - 1),
                                get_frm(frmin, i + sx),
                                get_frm(frmin, i + sx + 1)));

                        ++i;
                }
                ++i; ++i;       /* two border pixels are skipped */
        }
}

static inline
frm_t edge_operator(frm_t k0, frm_t k1, frm_t k2, frm_t k3, frm_t k4, frm_t k5,
                frm_t k6, frm_t k7, frm_t k8)
{
        /* Sobel edge detector */

        const int p = k2 + 2*k5 + k8 - k0 - 2*k3 - k6;
        const int q = k6 + 2*k7 + k8 - k0 - 2*k1 - k2;

        (void) k4;

        return 128 + 1.0/8*abs(p) + 1.0/8*abs(q);
}

void apply_median(int sx, int sy, const frm_t *frmin, frm_t *frmout)
{
        int i;

        /* The borders are not filtered */
        for (i = 0; i < sx; ++i) {
                update_frm(frmout, i, get_frm(frmin, i)); /* upper border */
                update_frm(frmout, (sy-1)*sx+i, /* bottom border */
                        get_frm(frmin, (sy-1)*sx+i));
        }

        for (i = 0; i < sy; ++i) {
                update_frm(frmout, i*sx, get_frm(frmin, i*sx)); /* left border */
                update_frm(frmout, (i+1)*sx-1,
                        get_frm(frmin, (i+1)*sx-1)); /* right border */
        }

        apply_operator(sx, sy, frmin, frmout, paeth_median);
}

void apply_edge(int sx, int sy, const frm_t *frmin, frm_t *frmout)
{
        int i;

        /* The borders are not filtered */
        for (i = 0; i < sx; ++i) {
                update_frm(frmout, i, BLACK); /* upper border */
                update_frm(frmout, (sy-1)*sx+i, BLACK);/* bottom border */
        }

        for (i = 0; i < sy; ++i) {
                update_frm(frmout, i*sx, BLACK); /* left border */
                update_frm(frmout, (i+1)*sx-1, BLACK);
        }

        apply_operator(sx, sy, frmin, frmout, edge_operator);
}
