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

#include <string.h>
#include <unistd.h>
#include "frm.h"

inline frm_t *allocate_frm(int size)
{
        return ((frm_t *) malloc(size * sizeof(frm_t)));
}

inline void deallocate_frm(frm_t *frm)
{
        free(frm);
}

inline void update_frm(frm_t *frm, int i, frm_t val)
{
        if (frm != NULL)
                frm[i] = val;
}

inline frm_t get_frm(const frm_t *frm, int i)
{
        return frm[i];
}

inline void copy_frm(const frm_t *frm1, frm_t *frm2, int i)
{
        memcpy(frm2, frm1, i * sizeof(frm_t));
}

inline int read_frm(frm_t *frm, int frmsize, FILE *instream)
{
        int c, i;

        for (i = 0; i < frmsize; ++i) {
                if ((c = fgetc(instream)) == EOF)
                        break;

                update_frm(frm, i, (frm_t) c);
        }

        return i;
}

inline int write_frm(int fd, const frm_t *frm, int frmsize)
{
        return write(fd, frm, frmsize * sizeof(frm_t));
}
