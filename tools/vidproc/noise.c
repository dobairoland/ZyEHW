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

#include "noise.h"

int apply_salt_pepper(frm_t *frm, int frmsize, int nlevel)
{
        const long decision = RAND_MAX * ((float) nlevel)/100;
        int i;

        if (nlevel < SALTPEPPER_MIN || nlevel > SALTPEPPER_MAX) {
                fprintf(stderr, "Salt and pepper noise value should be "
                                "between %d %% and %d %%!\n", SALTPEPPER_MIN,
                                SALTPEPPER_MAX);
                return 0;
        }

        for (i = 0; i < frmsize; ++i) {
                if (rand() <= decision)
                        update_frm(frm, i, (rand() & 1) ? 0: ~((frm_t) 0));
        }

        return 1;
}
