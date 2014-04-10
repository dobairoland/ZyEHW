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

entity ram is
    generic (
        AWIDTH: integer:= 4;
        DWIDTH: integer:= 8
    );
    port (
        rdclk: in std_logic;
        rden: in std_logic;
        rdaddr: in std_logic_vector(AWIDTH-1 downto 0);
        do: out std_logic_vector(DWIDTH-1 downto 0);
        wrclk: in std_logic;
        wren: in std_logic;
        wraddr: in std_logic_vector(AWIDTH-1 downto 0);
        di: in std_logic_vector(DWIDTH-1 downto 0)
    );
end ram;

architecture behav_ram of ram is
    subtype data_t is std_logic_vector(DWIDTH-1 downto 0);
    type ram_t is array(0 to (2**AWIDTH)-1) of data_t;

    signal content: ram_t;
    signal do_reg: data_t;
begin
    process (wrclk)
    begin
        if wrclk'event and wrclk = '1' then
            if wren = '1' then
                content(to_integer(unsigned(wraddr))) <= di;
            end if;
        end if;
    end process;

    process (rdclk)
    begin
        if rdclk'event and rdclk = '1' then
            if rden = '1' then
                do_reg <= content(to_integer(unsigned(rdaddr)));
            end if;

            do <= do_reg;
        end if;
    end process;
end behav_ram;
