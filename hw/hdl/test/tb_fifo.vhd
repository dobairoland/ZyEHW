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

entity tb_fifo is
end tb_fifo;

architecture behav_tb_fifo of tb_fifo is
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

    constant t_wr: time:= 18 ns;
    constant t_rd: time:= 10 ns;
    constant data_low: integer:= 0;
    constant data_high: integer:= (2**16)-1;

    signal rdclk: std_logic;
    signal wrclk: std_logic;
    signal rden: std_logic:= '0';
    signal wren: std_logic:= '0';
    signal write: std_logic;
    signal pre_datain, datain: fifo_t;
    signal dataout: fifo_t;
    signal rderr: std_logic;
    signal wrerr: std_logic;
    signal near_full: std_logic;
    signal half_full: std_logic;

begin
    write <= '0', '1' after 10*t_wr;

    -- In the axi_data core the write enable is written into a register. Here,
    -- the signal write mimics this: it writes the datain into a register and
    -- is similarly stored into another register.
    process (wrclk)
    begin
        if wrclk'event and wrclk = '1' then
            wren <= write;

            if write = '1' then
                datain <= pre_datain;
            end if;
        end if;
    end process;

    rden_proc: process (rdclk)
        variable readon: boolean:= false;
        variable i: integer;
        variable rd: std_logic;
    begin
        -- It waits for half_full and then it reads a frame from the fifo
        if (rdclk'event and rdclk = '1') then
            if (half_full = '1' and readon = false) then
                readon:= true;
                i:= 0;
            end if;

            if (readon = true) then
                rd := '1';

                if (i = 128*128-1) then
                    readon:= false;
                else
                    i:= i + 1;
                end if;
            else
                rd := '0';
            end if;

            rden <= rd;
        end if;
    end process;

    datain_proc: process
        variable input: integer:= data_low;
        variable input_v: std_logic_vector(2*cgp_t'length-1 downto 0);
    begin
        input_v:= std_logic_vector(to_unsigned(input, input_v'length));
        pre_datain.input <= input_v(1*cgp_t'length-1 downto 0*cgp_t'length);
        pre_datain.output <= input_v(2*cgp_t'length-1 downto 1*cgp_t'length);

        if input = data_high then
            input:= data_low;
        else
            input:= input + 1;
        end if;

        wait for t_wr;
    end process;

    rdclk_proc: process
        variable tmp_clk: std_logic:= '1';
    begin
        rdclk <= tmp_clk;
        tmp_clk := not tmp_clk;
        wait for t_rd / 2;
    end process;

    wrclk_proc: process
        variable tmp_clk: std_logic:= '1';
    begin
        wrclk <= tmp_clk;
        tmp_clk := not tmp_clk;
        wait for t_wr / 2;
    end process;

    fifo_i: fifo
    port map (
        rdclk => rdclk,
        wrclk => wrclk,
        rden => rden,
        wren => wren,
        datain => datain,
        dataout => dataout,
        rderr => rderr,
        wrerr => wrerr,
        near_full => near_full,
        half_full => half_full
    );
end behav_tb_fifo;
