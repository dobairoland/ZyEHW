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

entity tb_fifo_sync is
end tb_fifo_sync;

architecture behav_tb_fifo_sync of tb_fifo_sync is
    component fifo_sync is
        port (
            axi0_clk: in std_logic;
            axi1_clk: in std_logic;
            cgp_clk: in std_logic;
            axi0_data: in fifo_t;
            axi0_datawr: in std_logic;
            cgp_datard: in std_logic;
            cgp_data: out fifo_t;
            cgp_fifo_ready: out std_logic;
            cgp_half_full: out std_logic;
            axi0_near_full: out std_logic;
            axi1_fifo_rderr: out std_logic;
            axi1_fifo_wrerr: out std_logic
        );
    end component;

    signal axi0_clk: std_logic;
    signal axi1_clk: std_logic;
    signal cgp_clk: std_logic;
    signal axi0_data: fifo_t;
    signal axi0_datawr: std_logic;
    signal cgp_datard: std_logic;
    signal cgp_data: fifo_t;
    signal cgp_fifo_ready: std_logic;
    signal cgp_half_full: std_logic;
    signal axi0_near_full: std_logic;
    signal axi1_fifo_rderr: std_logic;
    signal axi1_fifo_wrerr: std_logic;
begin
    axi0_data.input <= (others => '0');
    axi0_data.output <= (others => '0');
    axi0_datawr <= '0';

    cgp_datard <= '0';

    fifo_sync_i: fifo_sync
        port map (
            axi0_clk => axi0_clk,
            axi1_clk => axi1_clk,
            cgp_clk => cgp_clk,
            axi0_data => axi0_data,
            axi0_datawr => axi0_datawr,
            cgp_datard => cgp_datard,
            cgp_data => cgp_data,
            cgp_fifo_ready => cgp_fifo_ready,
            cgp_half_full => cgp_half_full,
            axi0_near_full => axi0_near_full,
            axi1_fifo_rderr => axi1_fifo_rderr,
            axi1_fifo_wrerr => axi1_fifo_wrerr
        );

    cgp_clk_proc: process
        variable tmp_clk: std_logic:= '1';
    begin
        cgp_clk <= tmp_clk;
        tmp_clk := not tmp_clk;
        wait for 5 ns;
    end process;

    axi_clk_proc: process
        variable tmp_clk: std_logic:= '1';
    begin
        axi0_clk <= tmp_clk;
        axi1_clk <= tmp_clk;
        tmp_clk := not tmp_clk;
        wait for 15 ns;
    end process;
end behav_tb_fifo_sync;
