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

entity cgp_sync is
    port (
             cgp_clk: in std_logic;
             axi_clk: in std_logic;
             axi_start: in std_logic;
             axi_mux_chromosome_arr: in mux_chromosome_arr_t;
             cgp_fifo_almostfull: in std_logic;
             cgp_fifo_ready: in std_logic;
             cgp_data: in fifo_t;
             cgp_datard: out std_logic;
             axi_end: out std_logic;
             axi_start_rst: out std_logic;
             axi_frame_count: out frame_count_t;
             axi_fitness_arr: out fitness_arr_t
    );
end cgp_sync;

architecture struct_cgp_sync of cgp_sync is

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

    signal cgp_start, start_sync: std_logic;
    signal cgp_mux_chromosome_arr, chrom_sync: mux_chromosome_arr_t;
    signal cgp_fitness_arr, fitness_sync: fitness_arr_t;
    signal fitness_reg: fitness_arr_t:= (others => (others => '1'));
    signal cgp_fitness_wr: std_logic;
    signal cgp_start_rst, start_rst_sync: std_logic;
    signal end_arst, end_arst_sync, end_reg, end_sync: std_logic;
    signal cgp_frame_count, frame_count_sync: frame_count_t;
begin

    cgp_i: cgp
    port map (
                 clk => cgp_clk,
                 start => cgp_start,
                 mux_chromosome_arr => cgp_mux_chromosome_arr,
                 fifo_almostfull => cgp_fifo_almostfull,
                 fifo_ready => cgp_fifo_ready,
                 data => cgp_data,
                 datard => cgp_datard,
                 frame_count => cgp_frame_count,
                 fitness_arr => cgp_fitness_arr,
                 fitness_wr => cgp_fitness_wr,
                 start_rst => cgp_start_rst
    );

    process (cgp_clk)
    begin
        if (cgp_clk'event and cgp_clk = '1') then
            chrom_sync <= axi_mux_chromosome_arr;
            cgp_mux_chromosome_arr <= chrom_sync;

            start_sync <= axi_start;
            cgp_start <= start_sync;

            if (cgp_fitness_wr = '1') then
                fitness_reg <= cgp_fitness_arr;
            end if;
        end if;
    end process;

    process (cgp_clk, end_arst)
    begin
        if (end_arst = '1') then
            end_reg <= '0';
        else
            if (cgp_clk'event and cgp_clk = '1') then
                if (cgp_fitness_wr = '1') then
                    end_reg <= '1';
                end if;
            end if;
        end if;
    end process;

    process (axi_clk, cgp_start_rst)
    begin
        if (cgp_start_rst = '1') then
            axi_start_rst <= '1';
            start_rst_sync <= '1';
        else
            if (axi_clk'event and axi_clk = '1') then
                start_rst_sync <= '0';
                axi_start_rst <= start_rst_sync;
            end if;
        end if;
    end process;

    process (cgp_clk, axi_start)
    begin
        if (axi_start = '1') then
            end_arst <= '1';
            end_arst_sync <= '1';
        else
            if (cgp_clk'event and cgp_clk = '1') then
                end_arst_sync <= '0';
                end_arst <= end_arst_sync;
            end if;
        end if;
    end process;

    process (axi_clk)
    begin
        if (axi_clk'event and axi_clk = '1') then
            end_sync <= end_reg;
            axi_end <= end_sync;
            fitness_sync <= fitness_reg;
            axi_fitness_arr <= fitness_sync;
            frame_count_sync <= cgp_frame_count;
            axi_frame_count <= frame_count_sync;
        end if;
    end process;

end struct_cgp_sync;
