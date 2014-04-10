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

#ifndef OCM_H
#define OCM_H

#include "xparameters.h"
#include "xil_types.h"
#include "cache.h"

#define RUNS                            30

#define OCM_PTR                         (volatile u32 *)
#define OCM_BASE_ADDRESS                XPAR_PS7_RAM_1_S_AXI_BASEADDR
#define OCM_BASE_32b_POINTER            (OCM_PTR OCM_BASE_ADDRESS)

#define OCM_CPU0_RUNNING                (*(OCM_PTR (OCM_BASE_32b_POINTER + 0)))
#define OCM_CPU1_RUNNING                (*(OCM_PTR (OCM_BASE_32b_POINTER + 1)))
#define OCM_FRAME_COUNTER               (*(OCM_PTR (OCM_BASE_32b_POINTER + 2)))

#define OCM_DISABLE_CACHE       set_tlb_attr(OCM_BASE_ADDRESS, L1_NOCACHE);

#endif
