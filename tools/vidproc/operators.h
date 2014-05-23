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

#ifndef OPERATORS_H
#define OPERATORS_H

#include "frm.h"

extern void apply_void(frm_t *frm, int frmsize);
extern void apply_median(int sx, int sy, const frm_t *frmin, frm_t *frmout);
extern void apply_edge(int sx, int sy, const frm_t *frmin, frm_t *frmout);

#endif
