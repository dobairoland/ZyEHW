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

entity cgp is
    port (
             clk: in std_logic;
             start: in std_logic;
             mux_chromosome_arr: in mux_chromosome_arr_t;
             fifo_almostfull: in std_logic;
             fifo_ready: in std_logic;
             data: in fifo_t;
             datard: out std_logic;
             frame_count: out frame_count_t;
             fitness_arr: out fitness_arr_t;
             fitness_wr: out std_logic;
             start_rst: out std_logic
         );
end cgp;

architecture struct_cgp of cgp is
    component ram_oen is
        generic (
            AWIDTH: integer;
            DWIDTH: integer
        );
        port (
            rdclk: in std_logic;
            rden: in std_logic;
            rdaddr: in std_logic_vector(AWIDTH-1 downto 0);
            oen: in std_logic;
            do: out std_logic_vector(DWIDTH-1 downto 0);
            wrclk: in std_logic;
            wren: in std_logic;
            wraddr: in std_logic_vector(AWIDTH-1 downto 0);
            di: in std_logic_vector(DWIDTH-1 downto 0)
        );
    end component;

    component fifo_reader is
        port (
            clk: in std_logic;
            fitness_wr: in std_logic;
            eval_start: in std_logic;
            fifo_rdy: in std_logic;
            fifo_almostfull: in std_logic;
            frame_count: out frame_count_t;
            fifo_rden: out std_logic;
            ram_wren: out std_logic;
            ram_wraddr: out img_addr_t;
            start_en: out std_logic
        );
    end component;

    component image_window is
        port (
            clk: in std_logic;
            en: in std_logic;
            input: in cgp_t;
            redundant_kernel_pixels: out cgp_input_redundant_t
        );
    end component;

    component image_reader is
        port (
            clk: in std_logic;
            start: in std_logic;
            start_en: in std_logic;
            eval_started: out std_logic;
            pe_start: out std_logic;
            ram_rden: out std_logic;
            ram_rdaddr: out img_addr_t
        );
    end component;

    component parallel_pe_fitness is
        port (
            clk: in std_logic;
            pe_start: in std_logic;
            mux_chromosome_arr: in mux_chromosome_arr_t;
            redundant_kernel_pixels: in cgp_input_redundant_t;
            correct: in cgp_t;
            fitness_arr: out fitness_arr_t;
            fitness_wr: out std_logic
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

    signal ram_rden, ram_wren: std_logic;
    signal ram_rdaddr, ram_wraddr: img_addr_t;
    signal correct, corrupt: cgp_t;
    signal start_en, eval_started: std_logic;
    signal int_fitness_wr: std_logic;
    signal redundant_kernel_pixels: cgp_input_redundant_t;
    signal delayed_correct: cgp_t;
    signal pe_start: std_logic;
begin
    fitness_wr <= int_fitness_wr;
    start_rst <= pe_start;

    correct_ram: ram_oen
    generic map (
        AWIDTH => img_addr_size,
        DWIDTH => cgp_t'length
    )
    port map (
        rdclk => clk,
        rden => ram_rden,
        rdaddr => ram_rdaddr,
        oen => ram_rden,
        do => correct,
        wrclk => clk,
        wren => ram_wren,
        wraddr => ram_wraddr,
        di => data.output
    );

    corrupt_ram: ram_oen
    generic map (
        AWIDTH => img_addr_size,
        DWIDTH => cgp_t'length
    )
    port map (
        rdclk => clk,
        rden => ram_rden,
        rdaddr => ram_rdaddr,
        oen => ram_rden,
        do => corrupt,
        wrclk => clk,
        wren => ram_wren,
        wraddr => ram_wraddr,
        di => data.input
    );

    fifo_reader_i: fifo_reader
    port map (
        clk => clk,
        fitness_wr => int_fitness_wr,
        eval_start => eval_started,
        fifo_rdy => fifo_ready,
        fifo_almostfull => fifo_almostfull,
        frame_count => frame_count,
        fifo_rden => datard,
        ram_wren => ram_wren,
        ram_wraddr => ram_wraddr,
        start_en => start_en
    );

    correct_sequencer: output_pipeline
    generic map (
        DEPTH => correct_delay_in_window
    )
    port map(
        clk => clk,
        en => ram_rden,
        input => correct,
        output => delayed_correct
    );

    corrupt_sequencer: image_window
    port map(
        clk => clk,
        en => ram_rden,
        input => corrupt,
        redundant_kernel_pixels => redundant_kernel_pixels
    );

    image_reader_i: image_reader
    port map (
        clk => clk,
        start => start,
        start_en => start_en,
        eval_started => eval_started,
        pe_start => pe_start,
        ram_rden => ram_rden,
        ram_rdaddr => ram_rdaddr
    );

    parallel_pe_fitness_i: parallel_pe_fitness
    port map (
        clk => clk,
        pe_start => pe_start,
        mux_chromosome_arr => mux_chromosome_arr,
        redundant_kernel_pixels => redundant_kernel_pixels,
        correct => delayed_correct,
        fitness_arr => fitness_arr,
        fitness_wr => int_fitness_wr
     );
end struct_cgp;
