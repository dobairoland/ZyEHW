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

entity fitness_unit is
    port (
        clk: in std_logic;
        rst: in std_logic;
        en: in std_logic;
        filtered_pixel: in cgp_t;
        correct_data: in cgp_t;
        fitness: out fitness_t
    );
end fitness_unit;

architecture behav_fitness_unit of fitness_unit is
    signal fitness_reg: fitness_t;
    signal abs_difference: cgp_t;

    signal difference,
        tmp_xored_difference,
        extended_difference,
        extended_difference_pipelined,
        tmp_abs_difference: std_logic_vector(cgp_t'length downto 0);
    signal xored_difference: cgp_t;
    signal sign, sign_pipelined: std_logic;
begin
    extended_difference <= (others => difference(cgp_t'length));
    extended_difference_pipelined <= (others => sign);
    tmp_xored_difference <= difference xor extended_difference;
    tmp_abs_difference <= std_logic_vector(signed('0' & xored_difference) -
                          signed(extended_difference_pipelined));

    process (clk)
    begin
        if (clk'event and clk = '1') then
            if (rst = '1') then
                difference <= (others => '0');
                xored_difference <= (others => '0');
                sign <= '0';
                abs_difference <= (others => '0');
                fitness_reg <= (others => '0');
            elsif (en = '1') then
                difference <= std_logic_vector(signed('0' & filtered_pixel) -
                              signed('0' & correct_data));
                xored_difference <= tmp_xored_difference(cgp_t'range);
                sign <= difference(cgp_t'length);
                abs_difference <= tmp_abs_difference(cgp_t'range);
                fitness_reg <= std_logic_vector(unsigned(fitness_reg) +
                               unsigned(abs_difference));
            end if;
        end if;
    end process;

    fitness <= fitness_reg;
end behav_fitness_unit;
