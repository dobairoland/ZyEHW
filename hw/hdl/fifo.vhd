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
use ieee.math_real.all;

use work.zyehw_pkg.all;

entity fifo is
    port (
        rdclk: in std_logic;
        wrclk: in std_logic;
        rden: in std_logic;
        wren: in std_logic;
        datain: in fifo_t;
        dataout: out fifo_t;
        rderr: out std_logic;
        wrerr: out std_logic;
        near_full: out std_logic;
        half_full: out std_logic
    );
end fifo;

architecture struct_fifo of fifo is
    component ram is
        generic (
            AWIDTH: integer;
            DWIDTH: integer
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
    end component;

    component gray_counter is
        generic (
            BITS: integer
        );
        port (
            clk: in std_logic;
            en: in std_logic;
            count: out std_logic_vector(BITS-1 downto 0);
            graycount: out std_logic_vector(BITS-1 downto 0)
        );
    end component;

    component fifo_flags is
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
    end component;

    constant addrwidth: integer:= integer(ceil(log2(real(2*img_pixels))));
    constant datawidth: integer:= 2*cgp_t'length;
    constant counterbits: integer:= addrwidth+1; -- To distinguish between
                                                 -- full and empty

    signal tmp_di, tmp_do: std_logic_vector(datawidth-1 downto 0);
    signal rdaddr, wraddr: std_logic_vector(addrwidth-1 downto 0);
    signal readcount, writecount: std_logic_vector(counterbits-1 downto 0);
    signal greadcount, gwritecount: std_logic_vector(counterbits-1 downto 0);
begin
    tmp_di(1*cgp_t'length-1 downto 0*cgp_t'length) <= datain.input;
    tmp_di(2*cgp_t'length-1 downto 1*cgp_t'length) <= datain.output;
    dataout.input <= tmp_do(1*cgp_t'length-1 downto 0*cgp_t'length);
    dataout.output <= tmp_do(2*cgp_t'length-1 downto 1*cgp_t'length);

    rdaddr <= readcount(rdaddr'range);
    wraddr <= writecount(wraddr'range);

    ram_i: ram
    generic map (
        AWIDTH => addrwidth,
        DWIDTH => datawidth
    )
    port map (
        rdclk => rdclk,
        rden => rden,
        rdaddr => rdaddr,
        do => tmp_do,
        wrclk => wrclk,
        wren => wren,
        wraddr => wraddr,
        di => tmp_di
    );

    read_address: gray_counter
    generic map (
        BITS => counterbits
    )
    port map (
        clk => rdclk,
        en => rden,
        count => readcount,
        graycount => greadcount
    );

    write_address: gray_counter
    generic map (
        BITS => counterbits
    )
    port map (
        clk => wrclk,
        en => wren,
        count => writecount,
        graycount => gwritecount
    );

    fifo_flags_i: fifo_flags
    generic map (
        AWIDTH => counterbits
    )
    port map (
        rdclk => rdclk,
        wrclk => wrclk,
        rden => rden,
        wren => wren,
        rdaddr => readcount,
        wraddr => writecount,
        grdaddr => greadcount,
        gwraddr => gwritecount,
        rderr => rderr,
        wrerr => wrerr,
        near_full => near_full,
        half_full => half_full
    );
end struct_fifo;
