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

static mut_t indiv_mut_arr[CGP_LAMBDA];
static mut_t indiv_mut_col_arr[CGP_LAMBDA];

/* two populations - swapping just the index */
static cgp_population_t population[2];
static unsigned short activepopulation = 0; /* swap index */

static cgp_indiv_t *alpha = NULL;
static fitness_t alphafitness = ~((fitness_t) 0);

#ifdef ADVEA
static unsigned short parent_change = 0;
#endif

static XTime cgp_time_acc = 0;

void reset_cgp_time()
{
        cgp_time_acc = 0;
}

XTime get_cgp_time()
{
        return cgp_time_acc;
}

static inline void init_mut_array()
{
        int i;

        for (i = 0; i < CGP_LAMBDA; ++i) {
                indiv_mut_arr[i] = NOT_MUTATED;
                indiv_mut_col_arr[i] = 0;
        }
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

#ifdef ADVEA
static void evaluate_popul_finer_reconfig(u32 *based_on_frame)
{
        cgp_indiv_t *indiv;
        int i;
        fitness_t fitness_arr[CGP_LAMBDA];
        XTime start, end;

        for (i = 1; i < (1 + CGP_LAMBDA); ++i) {
                indiv = &population[activepopulation][i];
                indiv_to_bitstream(indiv, i - 1);
        }

        reconfig_population();

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

        if (parent_change) {
                /* inverse reconfiguration is needed */
                for (i = 1; i < (1 + CGP_LAMBDA); ++i) {
                        /* the previous elit is established in the
                         * programmable logic (at each position) */

                        indiv = &population[activepopulation][i];

                        /* the reconfiguration will be done only where changes
                         * have been made */
                        alpha->mut_bank = indiv->mut_bank;
                        alpha->mut_col = indiv->mut_col;
                        indiv_to_bitstream(alpha, i - 1);
                }

                reconfig_population();

                alpha->mut_bank = NOT_MUTATED;
                init_mut_array();
        }

#ifdef PARENT_SEC_GENER
        parent_change ^= 1;
#endif
}
#endif

u32 init_popul()
{
        int i;
        u32 frame;

        alpha = NULL;
        alphafitness = ~((fitness_t) 0);
        activepopulation = 0;

#ifdef PARENT_SEC_GENER
        parent_change = 0;
#else
        parent_change = 1;
#endif

#ifdef ADVEA
        init_mut_array();
#endif

        for (i = 1; i < (1 + CGP_LAMBDA); ++i)
                init_indiv(&population[activepopulation][i]);

        /* This is the worst possible fitness because this individual will not
         * be evaluated. */
        population[activepopulation][0].fitness = alphafitness;

        evaluate_popul(&frame);

        return frame;
}

void init_popul_with_elit()
{
        int i;

        /* the elit is copied everywhere */
        for (i = 1; i < (1 + CGP_LAMBDA); ++i) {
                indiv_to_fpga(alpha, i - 1);
        }
}

u32 new_popul()
{
        int i, j, k;
        cgp_indiv_t *prevalpha;
        cgp_indiv_t *indiv;
        u32 frame = 0;

#ifdef PARENT_SEC_GENER
        if (parent_change) {
                prevalpha = &population[activepopulation][0];
                activepopulation ^= 1;
                copy_indiv(alpha, &population[activepopulation][0]);
                alpha = &population[activepopulation][0];
        } else {
#endif
                prevalpha = alpha;
                activepopulation ^= 1;
                alpha = &population[activepopulation][0];
                copy_indiv(prevalpha, alpha);
#ifdef PARENT_SEC_GENER
        }
#endif

        for (i = 1, k = 0; i < (1 + CGP_LAMBDA); k = i++) {
                indiv = &population[activepopulation][i];

                copy_indiv(prevalpha, indiv);
#ifdef ADVEA
                indiv->mut_col = indiv_mut_col_arr[k];
                indiv->mut_bank = indiv_mut_arr[k];
#else
                (void) k; /* removes unused warning */
#endif

                for (j = 0; j < CGP_MUTATIONS; ++j)
                        mutate_indiv(indiv);

#ifdef ADVEA
                indiv_mut_col_arr[k] = indiv->mut_col;
                indiv_mut_arr[k] = indiv->mut_bank;
#endif
        }

#ifdef ADVEA
        evaluate_popul_finer_reconfig(&frame);
#else
        evaluate_popul(&frame);
#endif
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
