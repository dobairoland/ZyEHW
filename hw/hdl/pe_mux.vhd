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

entity pe_mux is
    port (
        clk: in std_logic;
        sel: in mux_sel_t;
        input: in mux_input_t;
        o: out cgp_t
    );
end pe_mux;

architecture behav_pe_mux of pe_mux is
    component mux1b_16_to_1 is
        port (
            clk: in std_logic;
            sel: in std_logic_vector(3 downto 0);
            input: in std_logic_vector(15 downto 0);
            o: out std_logic
        );
    end component;

    subtype mux16_input_t is std_logic_vector(15 downto 0);
    type input_arr_t is array (cgp_t'range) of mux16_input_t;

    signal input_arr: input_arr_t;
begin
    input_arr_row: for i in cgp_t'range generate
        input_arr_col: for j in 0 to mux_input_t'high generate
            input_arr(i)(j) <= input(j)(i);
        end generate;

        input_arr_zero_col: for j in mux_input_t'high+1 to mux16_input_t'high
        generate
            input_arr(i)(j) <= '0';
        end generate;
    end generate;

    mux_gen: for i in cgp_t'range generate
        mux_i: mux1b_16_to_1
        port map (
            clk => clk,
            sel => sel,
            input => input_arr(i),
            o => o(i)
        );
    end generate;
end behav_pe_mux;
