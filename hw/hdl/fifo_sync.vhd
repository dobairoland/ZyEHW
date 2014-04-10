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

entity fifo_sync is
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
end fifo_sync;

architecture struct_fifo_sync of fifo_sync is
    component fifo is
        port (
            rdclk: in std_logic;
            wrclk: in std_logic;
            rden: in std_logic;
            wren: in std_logic;
            datain: in fifo_t;
            dataout: out fifo_t;
            rderr: out std_logic;
            wrerr: out std_logic;
            near_full: out std_logic;
            half_full: out std_logic
        );
    end component;

    constant fifo_ready: std_logic:= '1';

    signal fifo_rderr, fifo_wrerr: std_logic;
    signal fifo_rderr_reg, fifo_wrerr_reg: std_logic:= '0';
    signal sync_rderr, sync_wrerr: std_logic:= '0';
    signal half_full, half_full_sync: std_logic:= '0';
begin
    cgp_fifo_ready <= fifo_ready;

    process (cgp_clk)
    begin
        if (cgp_clk'event and cgp_clk = '1') then
            half_full_sync <= half_full;
            cgp_half_full <= half_full_sync;

            if (fifo_rderr = '1') then
                fifo_rderr_reg <= '1';
            end if;
        end if;
    end process;

    process (axi1_clk)
    begin
        if (axi1_clk'event and axi1_clk = '1') then
            sync_rderr <= fifo_rderr_reg;
            axi1_fifo_rderr <= sync_rderr;

            sync_wrerr <= fifo_wrerr_reg;
            axi1_fifo_wrerr <= sync_wrerr;
        end if;
    end process;

    process (axi0_clk)
    begin
        if (axi0_clk'event and axi0_clk = '1') then
            if (fifo_wrerr = '1') then
                fifo_wrerr_reg <= '1';
            end if;
        end if;
    end process;

    fifo_i: fifo
    port map (
        rdclk => cgp_clk,
        wrclk => axi0_clk,
        rden => cgp_datard,
        wren => axi0_datawr,
        datain => axi0_data,
        dataout => cgp_data,
        rderr => fifo_rderr,
        wrerr => fifo_wrerr,
        near_full => axi0_near_full,
        half_full => half_full
    );
end struct_fifo_sync;
