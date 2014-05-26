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

#include "population.h"

#include <stdlib.h>
#include "xparameters.h"
#include "xil_printf.h"

typedef cgp_indiv_t cgp_population_t[1 + CGP_LAMBDA];

/* two populations - swapping just the index */
static cgp_population_t population[2];
static unsigned short activepopulation = 0; /* swap index */

static cgp_indiv_t *alpha = NULL;
static fitness_t alphafitness = ~((fitness_t) 0);

static XTime cgp_time_acc = 0;

void reset_cgp_time()
{
        cgp_time_acc = 0;
}

XTime get_cgp_time()
{
        return cgp_time_acc;
}

static void evaluate_popul(u32 *based_on_frame)
{
        cgp_indiv_t *indiv;
        int i;
        fitness_t fitness_arr[CGP_LAMBDA];
        XTime start, end;

        for (i = 1; i < (1 + CGP_LAMBDA); ++i) {
                indiv = &population[activepopulation][i];
                indiv_to_fpga(indiv, i - 1);
        }

        /* Evolution start */
        XTime_GetTime(&start);
        start_cgp();
        wait_cgp(fitness_arr, based_on_frame);
        XTime_GetTime(&end);
        cgp_time_acc += end - start;
        /* Evolution end */

        for (i = 1; i < (1 + CGP_LAMBDA); ++i) {
                indiv = &population[activepopulation][i];

                indiv->fitness = fitness_arr[i - 1];

                if (indiv->fitness <= alphafitness) {
                        alphafitness = indiv->fitness;
                        alpha = indiv;
                }
        }
}

u32 init_popul()
{
        int i;
        u32 frame;

        alpha = NULL;
        alphafitness = ~((fitness_t) 0);
        activepopulation = 0;

        for (i = 1; i < (1 + CGP_LAMBDA); ++i)
                init_indiv(&population[activepopulation][i]);

        /* This is the worst possible fitness because this individual will not
         * be evaluated. */
        population[activepopulation][0].fitness = alphafitness;

        evaluate_popul(&frame);

        return frame;
}

u32 new_popul()
{
        int i, j;
        cgp_indiv_t *prevalpha = alpha;
        cgp_indiv_t *indiv;
        u32 frame;

        activepopulation ^= 1;

        alpha = &population[activepopulation][0];
        copy_indiv(prevalpha, alpha);

        for (i = 1; i < (1 + CGP_LAMBDA); ++i) {
                indiv = &population[activepopulation][i];

                copy_indiv(prevalpha, indiv);

                for (j = 0; j < CGP_MUTATIONS; ++j)
                        mutate_indiv(indiv);
        }

        evaluate_popul(&frame);

        return frame;
}

const cgp_indiv_t *get_elit()
{
        return alpha;
}

unsigned long get_sec_time(XTime time)
{
        return time / (XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ
                        >> 1); /* XTime counts 2 cycles*/
}

void print_indiv_xml(const cgp_indiv_t *indiv)
{
        int i, j, k;
        lut_t msb = 0, lsb = 0;

        xil_printf("\t<fitness>0x%x</fitness>\n\r", indiv->fitness);

        for (i = 0; i < CGP_COL; ++i) {
                for (j = 0; j < CGP_ROW; ++j) {
                        const pe_t *pe = &indiv->pe_arr[i][j];

                        xil_printf("\t<pe col=\"%d\" row=\"%d\">\n\r"
                                        "\t<f0>0x%x</f0>\n\r"
                                        "\t<f1>0x%x</f1>\n\r"
                                        "\t<f2>0x%x</f2>\n\r"
                                        "\t<f3>0x%x</f3>\n\r"
                                        "\t<a>0x%x</a>\n\r"
                                        "\t<b>0x%x</b>\n\n\r",
                                        i, j, pe->f_b0, pe->f_b1, pe->f_b2,
                                        pe->f_b3, pe->mux_a, pe->mux_b);

                        function_to_bitstream(i, j, pe->f_b0, pe->f_b1,
                                        pe->f_b2, pe->f_b3);

                        for (k = 0; k < CGP_BIT; ++k) {
                                lut_from_bitstream(indiv, i, j, k, &msb, &lsb);

                                xil_printf("\t<l id=\"%d\"><msb>0x%.8x</msb>"
                                                "<lsb>0x%.8x</lsb></l>\n\r", k,
                                                msb, lsb);
                        }

                        xil_printf("\t</pe>\n\n\r");
                }
        }

        xil_printf("\t<filter_switch>0x%x</filter_switch>\n\r",
                        indiv->filter_switch);
        xil_printf("\t<out_select>0x%x</out_select>\n\r", indiv->out_select);
}
