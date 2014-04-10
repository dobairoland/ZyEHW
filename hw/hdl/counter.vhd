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

entity counter is
    generic (
        BITS: integer:= 4
    );
    port (
        clk: in std_logic;
        en: in std_logic;
        rst: in std_logic;
        count: out std_logic_vector(BITS-1 downto 0)
    );
end counter;

architecture behav_counter of counter is
    signal tmp_count: std_logic_vector(count'range):= (others => '0');
begin
    count <= tmp_count;

    process (clk)
    begin
        if clk'event and clk = '1' then
            if rst = '1' then
                tmp_count <= (others => '0');
            elsif en = '1' then
                tmp_count <= std_logic_vector(unsigned(tmp_count) + 1);
            end if;
        end if;
    end process;
end behav_counter;
