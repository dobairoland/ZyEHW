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

entity tb_fitness_unit is
end tb_fitness_unit;

architecture behav_tb_fitness_unit of tb_fitness_unit is
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
    signal rst: std_logic;
    signal en: std_logic;
    signal filtered_pixel: cgp_t;
    signal correct_data: cgp_t;
    signal fitness: fitness_t;
    signal tmp_en: std_logic;
begin

    rst <= '1', '0' after 30 ns;
    tmp_en <= '0', '1' after 50 ns;
    filtered_pixel <= std_logic_vector(to_unsigned(5, cgp_t'length));
    correct_data <= std_logic_vector(to_unsigned(3, cgp_t'length));

    process (clk)
    begin
        if clk'event and clk = '1' then
            en <= tmp_en;
        end if;
    end process;

    fitness_unit_i: fitness_unit
    port map (
        clk => clk,
        rst => rst,
        en => en,
        filtered_pixel => filtered_pixel,
        correct_data => correct_data,
        fitness => fitness
    );

    clk_proc: process
        variable tmp_clk: std_logic:= '1';
    begin
        clk <= tmp_clk;
        tmp_clk := not tmp_clk;
        wait for 5 ns;
    end process;
end behav_tb_fitness_unit;
