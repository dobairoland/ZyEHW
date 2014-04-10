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
use ieee.math_real.all;

use work.zyehw_pkg.all;

entity fifo_flags is
    generic (
        AWIDTH: integer
    );
    port (
        rdclk: in std_logic;
        wrclk: in std_logic;
        rden: in std_logic;
        wren: in std_logic;
        rdaddr: in std_logic_vector(AWIDTH-1 downto 0);
        wraddr: in std_logic_vector(AWIDTH-1 downto 0);
        grdaddr: in std_logic_vector(AWIDTH-1 downto 0);
        gwraddr: in std_logic_vector(AWIDTH-1 downto 0);
        rderr: out std_logic;
        wrerr: out std_logic;
        near_full: out std_logic;
        half_full: out std_logic
    );
end fifo_flags;

architecture struct_fifo_flags of fifo_flags is
    component fifo_flags_int_sync is
        generic (
            AWIDTH: integer
        );
        port (
            rdclk: in std_logic;
            wrclk: in std_logic;
            rden: in std_logic;
            wren: in std_logic;
            rdaddr: in std_logic_vector(AWIDTH-1 downto 0);
            wraddr: in std_logic_vector(AWIDTH-1 downto 0);
            grdaddr: in std_logic_vector(AWIDTH-1 downto 0);
            gwraddr: in std_logic_vector(AWIDTH-1 downto 0);
            rden_del: out std_logic;
            wren_del: out std_logic;
            rdaddr_del: out std_logic_vector(AWIDTH-1 downto 0);
            wraddr_del: out std_logic_vector(AWIDTH-1 downto 0);
            wrclk_rdaddr: out std_logic_vector(AWIDTH-1 downto 0);
            rdclk_wraddr: out std_logic_vector(AWIDTH-1 downto 0)
        );
    end component;

    constant capacity: integer:= 2**(AWIDTH-1);
    constant near_full_count: integer:= capacity-100;
    constant half_full_count: integer:= capacity/2;

    signal rden_del: std_logic;
    signal wren_del: std_logic;
    signal rdaddr_del: std_logic_vector(AWIDTH-1 downto 0);
    signal wraddr_del: std_logic_vector(AWIDTH-1 downto 0);
    signal wrclk_rdaddr: std_logic_vector(AWIDTH-1 downto 0);
    signal rdclk_wraddr: std_logic_vector(AWIDTH-1 downto 0);

    signal diff: std_logic_vector(AWIDTH-1 downto 0):= (others => '0');

    signal rderr_reg: std_logic:= '0';
    signal wrerr_reg: std_logic:= '0';
    signal half_full_reg: std_logic:= '0';
    signal near_full_reg: std_logic:= '0';

    signal empty, full: std_logic;
    signal tmp_near_full, tmp_half_full: std_logic;
begin
    fifo_flags_int_sync_i: fifo_flags_int_sync
    generic map (
        AWIDTH => AWIDTH
    )
    port map (
        rdclk => rdclk,
        wrclk => wrclk,
        rden => rden,
        wren => wren,
        rdaddr => rdaddr,
        wraddr => wraddr,
        grdaddr => grdaddr,
        gwraddr => gwraddr,
        rden_del => rden_del,
        wren_del => wren_del,
        rdaddr_del => rdaddr_del,
        wraddr_del => wraddr_del,
        wrclk_rdaddr => wrclk_rdaddr,
        rdclk_wraddr => rdclk_wraddr
    );

    process (wrclk_rdaddr, wraddr_del, diff)
    begin
        -- The most significant bit is used to distinguish between full and
        -- empty
        if ((not(wrclk_rdaddr(wrclk_rdaddr'high))) &
        wrclk_rdaddr(wrclk_rdaddr'high-1 downto wrclk_rdaddr'low)) =
        wraddr_del then
            full <= '1';
        else
            full <= '0';
        end if;

        if diff >= std_logic_vector(to_unsigned(half_full_count, AWIDTH))
        then
            tmp_half_full <= '1';
        else
            tmp_half_full <= '0';
        end if;

        if diff >= std_logic_vector(to_unsigned(near_full_count, AWIDTH))
        then
            tmp_near_full <= '1';
        else
            tmp_near_full <= '0';
        end if;
    end process;

    process (rdclk_wraddr, rdaddr_del)
    begin
        if rdclk_wraddr = rdaddr_del then
            empty <= '1';
        else
            empty <= '0';
        end if;
    end process;

    process (wrclk)
        variable tmp_diff: std_logic_vector(diff'high+1 downto diff'low);
    begin
        if wrclk'event and wrclk = '1' then
            if wren_del = '1' then
                wrerr_reg <= full;
            end if;

            tmp_diff:= std_logic_vector(('0' & signed(wraddr_del)) - ('0' &
                     signed(wrclk_rdaddr)));
            -- If wradd_del is bigger then removing the sign (0) gives the
            -- difference. Otherwise, removing the negative sign (1) will give
            -- max. value minus absolute difference plus one (exactly what we
            -- want).
            diff <= tmp_diff(diff'range);

            near_full_reg <= tmp_near_full;
            half_full_reg <= tmp_half_full;
        end if;
    end process;

    process (rdclk)
    begin
        if rdclk'event and rdclk = '1' then
            if rden_del = '1' then
                rderr_reg <= empty;
            end if;
        end if;
    end process;

    rderr <= rderr_reg;
    wrerr <= wrerr_reg;
    half_full <= half_full_reg;
    near_full <= near_full_reg;
end struct_fifo_flags;
