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

#ifndef FRM_H
#define FRM_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

typedef uint8_t frm_t;

#define FRMBITS         (sizeof(frm_t)*8)
#define FRM_MAX         ((1<<FRMBITS)-1)

extern frm_t *allocate_frm(int size);
extern void deallocate_frm(frm_t *frm);
extern void update_frm(frm_t *frm, int i, frm_t val);
extern frm_t get_frm(const frm_t *frm, int i);
extern void copy_frm(const frm_t *frm1, frm_t *frm2, int i);
extern int read_frm(frm_t *frm, int frmsize, FILE *instream);
extern int write_frm(int fd, const frm_t *frm, int frmsize);

#endif
