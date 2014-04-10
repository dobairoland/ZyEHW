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
#include "xtime_l.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "xscugic.h"
#include "ocm.h"
#include "population.h"
#include "vrc.h"
#include "development.h"

#if 0
#define PRINT_BITSTREAM
#include "bitstream.h"
#endif

#define PL_INT_ID       XPAR_FABRIC_SYSTEM_IRQ_F2P_INTR

static XScuGic ic;
static XScuGic_Config *icconf;

#ifdef PRINT_BITSTREAM
void print_bitstream()
{
        const int size = size_of_lut_stream();
        int i;

        for (i = 0; i < size; ++i)
                xil_printf("%X\r\n", lut_stream[i]);
}
#endif

void interrupt_handler(void *data)
{
        if (fifo_read_error())
                xil_printf("FIFO read error!\n\r");

        if (fifo_write_error())
                xil_printf("FIFO write error!\n\r");
}

static inline void init_interrupt()
{
        Xil_ExceptionInit();

        if ((icconf = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID))
                        == NULL)
                return;

        if (XScuGic_CfgInitialize(&ic, icconf, icconf->CpuBaseAddress) !=
                        XST_SUCCESS)
                return;

        Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
                        (Xil_ExceptionHandler) XScuGic_InterruptHandler, &ic);

        if (XScuGic_Connect(&ic, PL_INT_ID, interrupt_handler,
                        NULL) != XST_SUCCESS)
                return;

        XScuGic_SetPriorityTriggerType(&ic, PL_INT_ID, 0, /*highest priority*/
                        3); /* 3 - rising edge, 1 - active high */

        XScuGic_Enable(&ic, PL_INT_ID);
        Xil_ExceptionEnable();
}

static inline void execute(int run)
{
        XTime time;
        u32 frame, init_frame, frame_before_start, generations;
        unsigned int seed;

        init_interrupt();

        XTime_GetTime(&time);
        seed = time;
        srand(seed);

        frame_before_start = read_frame_number();

        XTime_SetTime(0);

        while (!OCM_CPU0_RUNNING);

        OCM_CPU1_RUNNING = 1;

        /* If the program will run without PL reset then the first frame will
         * not be 0. */
        init_frame = init_popul();
        update_development(get_elit(), init_frame, time);

        for (generations = 1; OCM_CPU0_RUNNING; ++generations) {
                frame = new_popul();
                update_development(get_elit(), frame, generations);
        }

        XTime_GetTime(&time);

        if (init_frame > 0 && init_frame == frame_before_start) {
                /* The first frames should be removed because those belong to
                 * the previous run. The size of the FIFO implies that only
                 * one frame remain in the FIFO but it will influence one
                 * run (furthermore, these frames might be several times
                 * in the development list). */
                int removed_developments = remove_first_frames();
                const int removed_frames = 1;

                /* Consequently, the frames are renumbered and should start
                 * with number 0 */
                postproc_development(init_frame + removed_frames);

#if 0
                xil_printf("Frame before start: 0x%X. First frame: 0x%X. "
                                "I removed %d development items "
                                "(%d frames).\n\r",
                                frame_before_start, init_frame,
                                removed_developments, removed_frames);
                (void) run;
#else
                (void) removed_developments;
#endif
        }

        xil_printf("Run %d is finished.\n\r", run);

        next_development_run(seed, time, OCM_FRAME_COUNTER, generations);

        OCM_CPU1_RUNNING = 0;
}

static inline void setup_cache()
{
        Denable();
        OCM_DISABLE_CACHE
}

int main()
{
        int i;

        xil_printf("CPU 1 begins...\n\r");

        /* This CPU owns the UART */

        setup_cache();
        init_development();

        for (i = 0; i < RUNS; ++i)
                execute(i);

        print_best_development();
        dealloc_development();

        xil_printf("CPU 1 ends.\n\r");

        return XST_SUCCESS;
}
