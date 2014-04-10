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

entity gray2binary is
    generic (
        WIDTH: integer
    );
    port (
        clk: in std_logic;
        input: in std_logic_vector(WIDTH-1 downto 0);
        output: out std_logic_vector(WIDTH-1 downto 0)
    );
end gray2binary;

architecture behav_gray2binary of gray2binary is
    signal output_reg: std_logic_vector(output'range):= (others => '0');
begin
    output <= output_reg;

    process (clk)
        variable tmp_out: std_logic_vector(output'range);
    begin
        if clk'event and clk = '1' then
            tmp_out:= (others => '0');
            for i in output'range loop
                for j in i to output'high loop
                    tmp_out(i):= tmp_out(i) xor input(j);
                end loop;
            end loop;

            output_reg <= tmp_out;
        end if;
    end process;
end behav_gray2binary;
