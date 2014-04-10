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

library unisim;
use unisim.vcomponents.all;

use work.zyehw_pkg.all;

entity mux1b_16_to_1 is
    port (
        clk: in std_logic;
        sel: in std_logic_vector(3 downto 0);
        input: in std_logic_vector(15 downto 0);
        o: out std_logic
    );
end mux1b_16_to_1;

architecture behav_mux1b_16_to_1 of mux1b_16_to_1 is
    signal l0, l1, l2, l3, m0, m1: std_logic;
    signal tmp_o: std_logic;
begin
    -- Implementation based on XAPP522 Mux design techniques

    lut6_l0 : lut6_l
    generic map (
        INIT => X"FF00F0F0CCCCAAAA")
    port map (
        lo => l0,
        i0 => input(0),
        i1 => input(1),
        i2 => input(2),
        i3 => input(3),
        i4 => sel(0),
        i5 => sel(1)
    );

    lut6_l1 : lut6_l
    generic map (
        INIT => X"FF00F0F0CCCCAAAA")
    port map (
        lo => l1,
        i0 => input(4),
        i1 => input(5),
        i2 => input(6),
        i3 => input(7),
        i4 => sel(0),
        i5 => sel(1)
    );

    lut6_l2 : lut6_l
    generic map (
        INIT => X"FF00F0F0CCCCAAAA")
    port map (
        lo => l2,
        i0 => input(8),
        i1 => input(9),
        i2 => input(10),
        i3 => input(11),
        i4 => sel(0),
        i5 => sel(1)
    );

    lut6_l3 : lut6_l
    generic map (
        INIT => X"FF00F0F0CCCCAAAA")
    port map (
        lo => l3,
        i0 => input(12),
        i1 => input(13),
        i2 => input(14),
        i3 => input(15),
        i4 => sel(0),
        i5 => sel(1)
    );

    muxf7_1: muxf7_l
    port map (
        lo => m0,
        i0 => l0,
        i1 => l1,
        s => sel(2)
    );

    muxf7_2: muxf7_l
    port map (
        lo => m1,
        i0 => l2,
        i1 => l3,
        s => sel(2)
    );

    muxf8_i: muxf8
    port map (
        o => tmp_o,
        i0 => m0,
        i1 => m1,
        s => sel(3)
    );

    process (clk)
    begin
        if(clk'event and clk = '1') then
            o <= tmp_o;
        end if;
    end process;
end architecture;
