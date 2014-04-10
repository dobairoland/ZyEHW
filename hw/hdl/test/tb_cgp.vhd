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
use work.img_pkg.all;

entity tb_cgp is
end tb_cgp;

architecture behav_tb_cgp of tb_cgp is
    component cgp is
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
    end component;

    signal clk: std_logic;
    signal start, tmp_start: std_logic;
    signal mux_chromosome_arr, tmp_mux_chromosome_arr: mux_chromosome_arr_t;
    signal fifo_almostfull, tmp_fifo_almostfull: std_logic;
    signal fifo_ready, tmp_fifo_ready: std_logic;
    signal data, data_st1: fifo_t;
    signal datard: std_logic;
    signal frame_count: frame_count_t;
    signal fitness_arr: fitness_arr_t;
    signal fitness_wr: std_logic;
    signal start_rst: std_logic;
begin
    tmp_start <= '1', '0' after 200 us,
                 '1' after 350 us, '0' after 400 us,
                 '1' after 550 us, '0' after 600 us;
    tmp_fifo_almostfull <= '1';
    tmp_fifo_ready <= '1';

    tmp_mux_chromosome_arr(0).a_mux <=
        (0 => (7 => "0100", others => "0000"),
        1 => (7 => "0100", others => "0000"),
        others => (others => "0000")),

        (0 => (6 => "0100", 7 => "1001", others => "0000"),
        1 => (7 => "0100", others => "0000"),
        others => (others => "0000")) after 350 us,

        (0 => (7 => "0100", others => "0000"),
        1 => (7 => "0100", others => "0000"),
        others => (others => "0000")) after 550 us;

    tmp_mux_chromosome_arr(0).b_mux <=
        (0 => (7 => "0100", others => "0000"),
        1 => (7 => "0100", others => "0000"),
        others => (others => "0000")),

        (0 => (6 => "0100", 7 => "1001", others => "0000"),
        1 => (7 => "0100", others => "0000"),
        others => (others => "0000")) after 350 us,

        (0 => (7 => "0100", others => "0000"),
        1 => (7 => "0100", others => "0000"),
        others => (others => "0000")) after 550 us;

    tmp_mux_chromosome_arr(0).out_select <= "00";
    tmp_mux_chromosome_arr(0).filter_switch <= "01";

    -- The fitness should be 549088, 563571, 549088.

    process (clk)
        variable i: integer:= 0;
    begin
        if clk'event and clk = '1' then
            start <= tmp_start;

            fifo_almostfull <= tmp_fifo_almostfull;
            fifo_ready <= tmp_fifo_ready;
            mux_chromosome_arr <= tmp_mux_chromosome_arr;

            if datard = '1' then
                data_st1.input <= test_corrupt_image(i);
                data_st1.output <= test_correct_image(i);

                if i = test_img_t'high then
                    i:= 0;
                else
                    i:= i + 1;
                end if;
            end if;

            -- The new version of the FIFO ram does not have and output enable
            data <= data_st1;
        end if;
    end process;

    cgp_i: cgp
    port map (
        clk => clk,
        start => start,
        mux_chromosome_arr => mux_chromosome_arr,
        fifo_almostfull => fifo_almostfull,
        fifo_ready => fifo_ready,
        data => data,
        datard => datard,
        frame_count => frame_count,
        fitness_arr => fitness_arr,
        fitness_wr => fitness_wr,
        start_rst => start_rst
    );

    clk_proc: process
        variable tmp_clk: std_logic:= '1';
    begin
        clk <= tmp_clk;
        tmp_clk := not tmp_clk;
        wait for 5 ns;
    end process;
end behav_tb_cgp;
