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

entity binary2gray is
    generic (
        WIDTH: integer
    );
    port (
        clk: in std_logic;
        en: in std_logic;
        input: in std_logic_vector(WIDTH-1 downto 0);
        output: out std_logic_vector(WIDTH-1 downto 0)
    );
end binary2gray;

architecture behav_binary2gray of binary2gray is
        signal tmp_output: std_logic_vector(output'range):= (others => '0');
begin
    output <= tmp_output;

    process (clk)
    begin
        if clk'event and clk = '1' then
            if en = '1' then
                tmp_output <= input xor
                          ('0' & input(input'high downto input'low+1));
            end if;
        end if;
    end process;
end behav_binary2gray;
