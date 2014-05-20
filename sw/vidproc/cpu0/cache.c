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

#include "cache.h"
#include "xil_mmu.h"
#include "xil_cache.h"

void set_tlb_attr(u32 addr, u32 attr)
{
        Xil_SetTlbAttributes(addr, attr);
}

void Dflush()
{
        Xil_DCacheFlush();
}

void Denable()
{
        Xil_DCacheEnable();
}

void Ddisable()
{
        Xil_DCacheDisable();
}

void Dflush_range(u32 addr, u32 length)
{
        Xil_DCacheFlushRange(addr, length);
}
