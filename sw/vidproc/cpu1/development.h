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

#ifndef DEVELOPMENT_H
#define DEVELOPMENT_H

#include "indiv.h"
#include "xtime_l.h"

extern void init_development();
extern void dealloc_development();
extern void update_development(const cgp_indiv_t *indiv, u32 frame,
                u32 generations);
extern void postproc_development(u32 first_frame);
extern int remove_first_frames();
extern void next_development_run(unsigned int seed, XTime time, u32 frame,
                u32 generations);
extern void print_best_development();

#endif
