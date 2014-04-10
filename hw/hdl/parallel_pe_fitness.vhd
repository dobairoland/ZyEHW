-- Copyright (C) 2014 Roland Dobai
--
-- This file is part of ZyEHW.
--
-- ZyEHW is free software: you can redistribute it and/or modify it under the
-- terms of the GNU General Public License as published by the Free Software
-- Foundation, either version 3 of the License, or (at your option) any later
-- version.
--
-- ZyEHW is distributed in the hope that it will be useful, but WITHOUT ANY
-- WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
-- FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
-- details.
--
-- You should have received a copy of the GNU General Public License along
-- with ZyEHW. If not, see <http://www.gnu.org/licenses/>.

library ieee;
use ieee.std_logic_1164.all;

use work.zyehw_pkg.all;

entity parallel_pe_fitness is
    port (
        clk: in std_logic;
        pe_start: in std_logic;
        mux_chromosome_arr: in mux_chromosome_arr_t;
        redundant_kernel_pixels: in cgp_input_redundant_t;
        correct: in cgp_t;
        fitness_arr: out fitness_arr_t;
        fitness_wr: out std_logic
    );
end parallel_pe_fitness;

architecture struct_parallel_pe_fitness of parallel_pe_fitness is
    component pe_array is
        generic (
            COL: integer;
            ROW: integer;
            SLICE_X_BASE: integer;
            SLICE_Y_BASE: integer;
            SLICE_X_STEP: integer;
            SLICE_X_COLUMNS: integer
        );
        port (
            clk: in std_logic;
            input: in  cgp_input_t;
            mux_chromosome: in mux_chromosome_t;
            filtered: out cgp_t
        );
    end component;

    component fitness_controler is
        port (
            clk: in std_logic;
            pe_start: in std_logic;
            fitness_wr: out std_logic;
            fitness_en: out std_logic;
            fitness_rst: out std_logic
        );
    end component;

    component fitness_unit is
        port (
            clk: in std_logic;
            rst: in std_logic;
            en: in std_logic;
            filtered_pixel: in cgp_t;
            correct_data: in cgp_t;
            fitness: out fitness_t
        );
    end component;

    component output_pipeline is
        generic (
            DEPTH: integer
        );
        port (
            clk: in std_logic;
            en: in std_logic;
            input: in cgp_t;
            output: out cgp_t
        );
    end component;

    type filtered_arr_t is array(0 to population-1) of cgp_t;

    constant bottom_index: integer:= 0;
    constant pe_array_in_row: integer:= 4;
    constant always_enabled: std_logic:= '1';
    constant output_pipeline_depth: integer:= columns + -- PE columns
                                              columns + -- PE mux columns
                                              1;        -- output stage

    signal delayed_reference: cgp_t;
    signal filtered_arr: filtered_arr_t;
    signal fitness_en: std_logic;
    signal fitness_rst: std_logic;
begin
    parallel_pe_array: for i in 0 to population-1 generate

        bottom_row: if i < pe_array_in_row generate
            pe_array_i: pe_array
            generic map (
                COL => columns,
                ROW => rows,
                SLICE_X_BASE => 1 + 32*i,
                SLICE_Y_BASE => ((bottom_index+1) * slice_per_row) - 1,
                SLICE_X_STEP => 10,
                SLICE_X_COLUMNS => 2
            )
            port map (
                clk => clk,
                input => redundant_kernel_pixels(i),
                mux_chromosome => mux_chromosome_arr(i),
                filtered => filtered_arr(i)
            );
        end generate;

        right_column: if i >= pe_array_in_row generate
            pe_array_i: pe_array
            generic map (
                COL => columns,
                ROW => rows,
                SLICE_X_BASE => 97,
                SLICE_Y_BASE =>
                    ((i-pe_array_in_row+2)*(bottom_index+1)*slice_per_row) - 1,
                SLICE_X_STEP => 10,
                SLICE_X_COLUMNS => 2
            )
            port map (
                clk => clk,
                input => redundant_kernel_pixels(i),
                mux_chromosome => mux_chromosome_arr(i),
                filtered => filtered_arr(i)
            );
        end generate;

        fitness_unit_i: fitness_unit
        port map (
            clk => clk,
            rst => fitness_rst,
            en => fitness_en,
            filtered_pixel => filtered_arr(i),
            correct_data => delayed_reference,
            fitness => fitness_arr(i)
        );
    end generate;

    output_pipeline_i: output_pipeline
    generic map (
        DEPTH => output_pipeline_depth
    )
    port map (
        clk => clk,
        en => always_enabled,
        input => correct,
        output => delayed_reference
    );

    fitness_controler_i: fitness_controler
    port map (
        clk => clk,
        pe_start => pe_start,
        fitness_wr => fitness_wr,
        fitness_en => fitness_en,
        fitness_rst => fitness_rst
    );
end struct_parallel_pe_fitness;
