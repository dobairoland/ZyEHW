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

#include "development.h"

#include <stdlib.h>
#include "xil_printf.h"
#include "population.h"
#include "ocm.h"

#define STORE_ALL_FRAMES_PRINT_ONLY_SOME

#ifdef STORE_ALL_FRAMES_PRINT_ONLY_SOME
#define PRINT_ALL_BOUNDARY      10
#define PRINT_ONLY              500
#endif

typedef struct development_struct {
        cgp_indiv_t indiv;
        u32 refframe;
        u32 generations;
        struct development_struct *next;
} development_t;

typedef struct development_run_struct {
        development_t *first;
        development_t *last;
        unsigned int seed;
        XTime time;
        XTime vrc_time;
        XTime dpr_time;
        XTime cgp_time;
        u32 frame;
        u32 generations;
} development_run_t;

static development_run_t development_run[RUNS];
static short active_run = 0;

void init_development()
{
        int i;

        for (i = 0; i < RUNS; ++i) {
                development_run[i].first = NULL;
                development_run[i].last = NULL;
        }
}

void dealloc_development()
{
        int i;

        for (i = 0; i < RUNS; ++i) {
                while (development_run[i].first != NULL) {
                        development_t *next = development_run[i].first->next;
                        free(development_run[i].first);
                        development_run[i].first = next;
                }

                development_run[i].last = NULL;
        }
}

static inline
void update_last(const cgp_indiv_t *indiv, u32 frame, u32 generations)
{
        copy_indiv(indiv, &development_run[active_run].last->indiv);
        development_run[active_run].last->refframe = frame;
        development_run[active_run].last->generations = generations;
}

static inline
void add_development(const cgp_indiv_t *indiv, u32 frame, u32 generations)
{
        development_t *new_item = malloc(sizeof(development_t));

        if (!new_item) {
                xil_printf("Malloc for development failed!\n\r");
                return;
        }

        new_item->next = NULL;

        if (development_run[active_run].last == NULL)
                development_run[active_run].first = new_item;
        else
                development_run[active_run].last->next = new_item;

        development_run[active_run].last = new_item;

        update_last(indiv, frame, generations);
}

static inline int is_saved(u32 frame)
{
        return ((development_run[active_run].last != NULL) &&
                        (frame == development_run[active_run].last->refframe));
}

static inline int changes(const cgp_indiv_t *indiv)
{
        return ((development_run[active_run].last == NULL) ||
        (!equal_indivs(&development_run[active_run].last->indiv, indiv)));
}

void update_development(const cgp_indiv_t *indiv, u32 frame, u32 generations)
{
        if (is_saved(frame)) {
                update_last(indiv, frame, generations);
#ifdef STORE_ALL_FRAMES_PRINT_ONLY_SOME
        } else
#else
        } else if (changes(indiv))
#endif
                add_development(indiv, frame, generations);
}

void postproc_development(u32 first_frame)
{
        development_t *p;

        for (p = development_run[active_run].first; p != NULL; p = p->next)
                p->refframe -= first_frame;
}

int remove_first_frames()
{
        int retval = 0;

        if (development_run[active_run].first != NULL) {
                const u32 frame = development_run[active_run].first->refframe;

                while (development_run[active_run].first != NULL &&
                development_run[active_run].first->refframe == frame) {
                        ++retval;
                        development_t *next =
                                development_run[active_run].first->next;
                        free(development_run[active_run].first);
                        development_run[active_run].first = next;
                }
        }

        if (development_run[active_run].first == NULL)
                development_run[active_run].last = NULL;

        return retval;
}

static inline
void print_development(int run)
{
        development_t *p;

#ifdef STORE_ALL_FRAMES_PRINT_ONLY_SOME
        int selection = (development_run[run].last == NULL) ? 1 :
        development_run[run].last->refframe/(PRINT_ONLY-PRINT_ALL_BOUNDARY);

        if (selection == 0)
                selection = 1;
#endif

        xil_printf("<?xml version=\"1.0\"?>\n\r<cgp_for_frames col=\"%d\" "
                        "row=\"%d\" bit=\"%d\">\n\r",
                        CGP_COL, CGP_ROW, CGP_BIT);

        for (p = development_run[run].first; p != NULL; p = p->next) {
#ifdef STORE_ALL_FRAMES_PRINT_ONLY_SOME
                if ((p->refframe > PRINT_ALL_BOUNDARY) &&
                                (p->refframe % selection != 0))
                        continue;
#endif
                print_indiv_xml(&p->indiv, p->generations, p->refframe);
        }

        xil_printf("<mutations>%d</mutations>\n\r", CGP_MUTATIONS);
        xil_printf("<lambda>%d</lambda>\n\r", CGP_INDIVS-1);
        xil_printf("<seed>0x%X</seed>\n\r", development_run[run].seed);
        xil_printf("<frames>0x%x</frames>\n\r", development_run[run].frame);
        xil_printf("<generations>0x%x</generations>\n\r",
                        development_run[run].generations);
        xil_printf("<total_time_sec>%ld</total_time_sec>\n\r",
                        get_sec_time(development_run[run].time));
        xil_printf("<vrc_time_sec>%ld</vrc_time_sec>\n\r",
                        get_sec_time(development_run[run].vrc_time));
        xil_printf("<dpr_time_sec>%ld</dpr_time_sec>\n\r",
                        get_sec_time(development_run[run].dpr_time));
        xil_printf("<cgp_time_sec>%ld</cgp_time_sec>\n\r",
                        get_sec_time(development_run[run].cgp_time));

        xil_printf("</cgp_for_frames>\n\r");
}

void next_development_run(unsigned int seed, XTime time, u32 frame,
                u32 generations)
{
        development_run[active_run].seed = seed;
        development_run[active_run].time = time;
        development_run[active_run].frame = frame;
        development_run[active_run].generations = generations;

        development_run[active_run].vrc_time = get_vrc_time();
        development_run[active_run].dpr_time = get_dpr_time();
        development_run[active_run].cgp_time = get_cgp_time();

        ++active_run;
}

static inline
unsigned long get_fitness_sum(int i)
{
        development_t *p;
        unsigned long ret = 0;

        for (p = development_run[i].first; p != NULL; p = p->next)
                ret += p->indiv.fitness;

        return ret;
}

void print_best_development()
{
        int i;
        int best_run = 0;
        unsigned long best_fitness_sum = get_fitness_sum(0);

#if 0
        xil_printf("Run %d has fitness sum 0x%X.\n\r", 0, best_fitness_sum);
#endif

        for (i = 1; i < RUNS; ++i) {
                unsigned long fitness_sum = get_fitness_sum(i);

#if 0
                xil_printf("Run %d has fitness sum 0x%X.\n\r", i, fitness_sum);
#endif

                if (fitness_sum < best_fitness_sum) {
                        best_fitness_sum = fitness_sum;
                        best_run = i;
                }
        }

#if 0
        xil_printf("Run %d will be printed.\n\r", best_run);
#endif

        print_development(best_run);

#ifdef STORE_ALL_FRAMES_PRINT_ONLY_SOME
        for (i = 0; i < RUNS; ++i) {
                development_t *p;

                xil_printf("\n\rHere are the fitness values for all frames "
                                "in run %d:\n\r", i);

                for (p = development_run[i].first; p != NULL; p = p->next)
                        xil_printf("%d\n\r", p->indiv.fitness);
        }
#endif
}
