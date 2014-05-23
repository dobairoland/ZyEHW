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
#include "population.h"
#include "vrc.h"
#include "train_data.h"

#if 0
#define PRINT_BITSTREAM
#include "bitstream.h"
#endif

#define RUNS            30
#define GENERATIONS     50000

#define PL_INT_ID       XPAR_FABRIC_SYSTEM_IRQ_F2P_INTR

static XScuGic ic;
static XScuGic_Config *icconf;

static fitness_t fit_arr[RUNS][GENERATIONS];
static cgp_indiv_t elits[RUNS];

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

static void init_interrupt()
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

static void execute(int run)
{
        XTime time;
        int i;
        unsigned int seed;

        XTime_GetTime(&time);
        seed = time;
        srand(seed);

        XTime_SetTime(0);
        reset_vrc_time();
        reset_dpr_time();
        reset_cgp_time();

        (void) init_popul();
        fit_arr[run][0] = get_elit()->fitness;

        for (i = 1; i < GENERATIONS; ++i) {
                (void) new_popul();
                fit_arr[run][i] = get_elit()->fitness;
        }

        XTime_GetTime(&time);

        copy_indiv(get_elit(), elits + run);

        xil_printf("Run %d is finished in %d seconds (VRC: %d, DPR: %d, "
                        "CGP: %d, seed: 0x%X).\n\r", run, get_sec_time(time),
                        get_sec_time(get_vrc_time()),
                        get_sec_time(get_dpr_time()),
                        get_sec_time(get_cgp_time()),
                        seed);
}

static void find_print_best()
{
        int i, best = 0;
        fitness_t best_fit = fit_arr[0][GENERATIONS-1];

        for (i = 1; i < RUNS; ++i) {
                if (best_fit > fit_arr[i][GENERATIONS-1]) {
                        best = i;
                        best_fit = fit_arr[i][GENERATIONS-1];
                }
        }

        xil_printf("<?xml version=\"1.0\"?>\n\r<cgp col=\"%d\" "
                        "row=\"%d\" bit=\"%d\">\n\r",
                        CGP_COL, CGP_ROW, CGP_BIT);

        print_indiv_xml(elits + best);

        xil_printf("<mutations>%d</mutations>\n\r", CGP_MUTATIONS);
        xil_printf("<lambda>%d</lambda>\n\r", CGP_LAMBDA);
        xil_printf("<generations>%d</generations>\n\r", GENERATIONS);

        xil_printf("</cgp>\n\r");
}

static void print_fitnesses()
{
        int i, j;

        for (i = 0; i < RUNS; ++i) {
                fitness_t last_fit = 0;

                xil_printf("\n\rFitness development during run %d:\n\r", i);

                for (j = 0; j < GENERATIONS; ++j) {
                        if (fit_arr[i][j] != last_fit) {
                                last_fit = fit_arr[i][j];
                                xil_printf("(%d, %d)\n\r", j, last_fit);
                        }
                }
        }
}

int main()
{
        int i;

        init_interrupt();

        xil_printf("CPU begins...\n\r");

        send_training_data();

        for (i = 0; i < RUNS; ++i)
                execute(i);

        find_print_best();
        print_fitnesses();

        xil_printf("CPU ends.\n\r");

        return XST_SUCCESS;
}
