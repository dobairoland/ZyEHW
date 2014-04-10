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

entity gray_counter is
    generic (
        BITS: integer
    );
    port (
        clk: in std_logic;
        en: in std_logic;
        count: out std_logic_vector(BITS-1 downto 0);
        graycount: out std_logic_vector(BITS-1 downto 0)
    );
end gray_counter;

architecture behav_gray_counter of gray_counter is
    component binary2gray is
        generic (
            WIDTH: integer
        );
        port (
            clk: in std_logic;
            en: in std_logic;
            input: in std_logic_vector(WIDTH-1 downto 0);
            output: out std_logic_vector(WIDTH-1 downto 0)
        );
    end component;

    signal tmp_count: std_logic_vector(count'range):= (others => '0');
    signal next_count: std_logic_vector(count'range);

    attribute max_fanout: integer;
    attribute max_fanout of tmp_count: signal is 5;
begin
    next_count <= std_logic_vector(unsigned(tmp_count) + 1);

    process (clk)
    begin
        if clk'event and clk = '1' then
            if en = '1' then
                tmp_count <= next_count;
            end if;
        end if;
    end process;

    count <= tmp_count;

    binary2gray_i: binary2gray
    generic map (
        WIDTH => BITS
    )
    port map (
        clk => clk,
        en => en,
        input => next_count,
        output => graycount
    );
end behav_gray_counter;
