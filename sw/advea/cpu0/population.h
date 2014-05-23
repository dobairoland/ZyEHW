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

#ifndef POPULATION_H
#define POPULATION_H

#include "indiv.h"
#include "xtime_l.h"

extern u32 init_popul();
extern u32 new_popul();
extern const cgp_indiv_t *get_elit();
extern void print_indiv_xml(const cgp_indiv_t *indiv);
extern unsigned long get_sec_time(XTime time);
extern XTime get_cgp_time();
extern void reset_cgp_time();

#endif
