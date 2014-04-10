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
use ieee.numeric_std.all;

use work.zyehw_pkg.all;

entity tb_fitness_controler is
end tb_fitness_controler;

architecture behav_tb_fitness_controler of tb_fitness_controler is
    component fitness_controler is
        port (
            clk: in std_logic;
            pe_start: in std_logic;
            fitness_wr: out std_logic;
            fitness_en: out std_logic;
            fitness_rst: out std_logic
        );
    end component;

    component output_pipeline is
        port (
            clk: in std_logic;
            input: in cgp_t;
            output: out cgp_t
        );
    end component;

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

    signal clk: std_logic;
    signal pe_start, tmp_pe_start: std_logic;
    signal fitness_wr: std_logic;
    signal fitness_en: std_logic;
    signal fitness_rst: std_logic;
    signal reference_in, tmp_reference_in, reference_out, filtered: cgp_t;
    signal corrupt_in, tmp_corrupt_in: cgp_input_t;
    signal mux_chromosome: mux_chromosome_t;
    signal fitness: fitness_t;
begin
    tmp_pe_start <= '0', '1' after 90 ns, '0' after 100 ns, '1' after 162 us,
                    '0' after 162010 ns;
    tmp_reference_in <= (0 => '1', others => '0') after 100 ns;
    tmp_corrupt_in <= (others => (others => '0')) after 100 ns;
    mux_chromosome.a_mux <= (others => (others => (others => '0')));
    mux_chromosome.b_mux <= (others => (others => (others => '0')));
    mux_chromosome.out_select <= (others => '0');
    mux_chromosome.filter_switch <= (others => '0');

    process (clk)
    begin
        if clk'event and clk = '1' then
            pe_start <= tmp_pe_start;
            reference_in <= tmp_reference_in;
            corrupt_in <= tmp_corrupt_in;
        end if;
    end process;

    fitness_controler_i: fitness_controler
    port map (
        clk => clk,
        pe_start => pe_start,
        fitness_wr => fitness_wr,
        fitness_en => fitness_en,
        fitness_rst => fitness_rst
    );

    output_pipeline_i: output_pipeline
    port map (
        clk => clk,
        input => reference_in,
        output => reference_out
    );

    pe_array_i: pe_array
    generic map (
        COL => columns,
        ROW => rows,
        SLICE_X_BASE => 49,
        SLICE_Y_BASE => 99,
        SLICE_X_STEP => 2,
        SLICE_X_COLUMNS => 2
    )
    port map (
        clk => clk,
        input => corrupt_in,
        mux_chromosome => mux_chromosome,
        filtered => filtered
    );

    fitness_unit_i: fitness_unit
    port map (
        clk => clk,
        rst => fitness_rst,
        en => fitness_en,
        filtered_pixel => filtered,
        correct_data => reference_out,
        fitness => fitness
    );

    clk_proc: process
        variable tmp_clk: std_logic:= '1';
    begin
        clk <= tmp_clk;
        tmp_clk := not tmp_clk;
        wait for 5 ns;
    end process;
end behav_tb_fitness_controler;
