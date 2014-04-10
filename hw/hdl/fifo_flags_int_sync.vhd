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

entity fifo_flags_int_sync is
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
end fifo_flags_int_sync;

architecture struct_fifo_flags_int_sync of fifo_flags_int_sync is
    component gray2binary is
        generic (
            WIDTH: integer
        );
        port (
            clk: in std_logic;
            input: in std_logic_vector(WIDTH-1 downto 0);
            output: out std_logic_vector(WIDTH-1 downto 0)
        );
    end component;

    subtype address_t is std_logic_vector(AWIDTH-1 downto 0);

    -- Double flopping for synchronization
    type sync_address_t is array (0 to 1) of address_t;

    -- After the synchronization the gray2binary conversion takes another
    -- clock cycle. Therefore, the delays should be longer by one clock cycle.
    type del_address_t is array (0 to 2) of address_t;

    -- Same delay as for the addresses:
    signal tmp_rden: std_logic_vector(2 downto 0):= (others => '0');
    signal tmp_wren: std_logic_vector(2 downto 0):= (others => '0');

    signal tmp_rdaddr, tmp_wraddr: del_address_t:= (others => (others => '0'));
    signal tmp_grdaddr, tmp_gwraddr: sync_address_t:= (others => (others =>
        '0'));
begin
    process (wrclk)
    begin
        if wrclk'event and wrclk = '1' then
            -- Just delayed:
            tmp_wren <= tmp_wren(tmp_wren'high-1 downto tmp_wren'low) & wren;
            tmp_wraddr <= wraddr &
                          tmp_wraddr(tmp_wraddr'low to tmp_wraddr'high-1);

            -- Sync into other clock domain:
            tmp_grdaddr <= grdaddr &
                          tmp_grdaddr(tmp_grdaddr'low to tmp_grdaddr'high-1);
        end if;
    end process;

    process (rdclk)
    begin
        if rdclk'event and rdclk = '1' then
            -- Just delayed:
            tmp_rden <= tmp_rden(tmp_rden'high-1 downto tmp_rden'low) & rden;
            tmp_rdaddr <= rdaddr &
                          tmp_rdaddr(tmp_rdaddr'low to tmp_rdaddr'high-1);

            -- Sync into other clock domain:
            tmp_gwraddr <= gwraddr &
                          tmp_gwraddr(tmp_gwraddr'low to tmp_gwraddr'high-1);
        end if;
    end process;

    gray_read: gray2binary
    generic map (
        WIDTH => AWIDTH
    )
    port map (
        clk => wrclk, -- it is already in the other clock domain
        input => tmp_grdaddr(tmp_grdaddr'high),
        output => wrclk_rdaddr
    );

    gray_write: gray2binary
    generic map (
        WIDTH => AWIDTH
    )
    port map (
        clk => rdclk, -- it is already in the other clock domain
        input => tmp_gwraddr(tmp_gwraddr'high),
        output => rdclk_wraddr
    );

    wren_del <= tmp_wren(tmp_wren'high);
    rden_del <= tmp_rden(tmp_rden'high);
    wraddr_del <= tmp_wraddr(tmp_wraddr'high);
    rdaddr_del <= tmp_rdaddr(tmp_rdaddr'high);
end struct_fifo_flags_int_sync;
