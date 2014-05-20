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

#ifndef CACHE_H
#define CACHE_H

#include "xil_types.h"

#define L1_NOCACHE                      0x14de2
/* L1 Cache table addresses 1 MBs. The upper 12 bits select the line
 * in the cache table. If it is ADDR then the offset is ADDRx4
 * (because the items are 32b words). The upper bits of the table
 * entry is ADDR followed by the arguments. L1_NOCACHE translates as
 * follows:
 * Ns = 0
 * 0 - section and not supersection
 * ng = 0 - global region
 * s = 1 - shareable
 * AP[2] = 0 - read/write full access
 * TEX = 0b100 - strongly ordered inner and outer policy
 * AP[1-0] = 0b11 - read/write full access
 * imp = 0
 * domain = 0b1111
 * xn = 0 - execute never
 * c = 0 - no cachable
 * b = 0 - no cachable
 * 0b10 - section or supersection
 */

extern void set_tlb_attr(u32 addr, u32 attr);
extern void Denable();
extern void Ddisable();
extern void Dflush();
extern void Dflush_range(u32 addr, u32 length);

#endif
