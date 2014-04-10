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
use work.lut_pkg.all;

entity pe_item is
    generic (
        OWN_ROW: integer;
        OWN_COLUMN: integer;
        SLICE_X_BASE: integer;
        SLICE_Y_BASE: integer;
        SLICE_X_STEP: integer;
        SLICE_X_COLUMNS: integer
    );
    port (
        clk: in std_logic;
        a: in std_logic_vector (7 downto 0);
        b: in std_logic_vector (7 downto 0);
        y: out std_logic_vector (7 downto 0)
    );
end pe_item;

architecture behav_pe_item of pe_item is

    -- _6LUT has 5 letters
    subtype lut_names is string(1 to 5);
    type bel_str_t is array (y'reverse_range) of lut_names;

    -- Arrangement like this (2 slice will be needed):
    -- y[7] -> D6LUT
    -- y[6] -> C6LUT
    -- y[5] -> B6LUT
    -- y[4] -> A6LUT
    -- y[3] -> D6LUT
    -- y[2] -> C6LUT
    -- y[1] -> B6LUT
    -- y[0] -> A6LUT
    constant bel_str: bel_str_t:= ("A6LUT", "B6LUT", "C6LUT", "D6LUT",
                                    "A6LUT", "B6LUT", "C6LUT", "D6LUT");

    --arrangement begin from SLICE_X{x_base}Y{y_base}
    constant x_base: integer := SLICE_X_BASE;
    constant y_base: integer := SLICE_Y_BASE;

    --number of free slices between PE slice columns
    constant x_slice_step: integer := SLICE_X_STEP;

    constant number_of_slice_columns: integer := SLICE_X_COLUMNS;
    constant number_of_slice_rows: integer :=
        columns*rows/number_of_slice_columns;

    constant slice_per_pe_item: integer := 2;

    constant one_dimensional_coordinate: integer := OWN_ROW+OWN_COLUMN*rows;

    constant new_column_coordinate: integer :=
        one_dimensional_coordinate/number_of_slice_rows;
    constant new_row_coordinate: integer := one_dimensional_coordinate mod
        number_of_slice_rows;

    constant default_mapping: string:= "I5:A6,I4:A5,I3:A4,I2:A3,I1:A2,I0:A1";

    constant new_column_is_odd: integer := new_column_coordinate mod 2;
    -- new_column_is_odd is 1 if the column is an odd number
    --  1 * (number_of_slice_rows - new_row_coordinate - 1) +
    --  0 * new_row_coordinate
    --  the coordinate is inversed in order to reduce the routing overhead
    -- new_column_is_odd is 0 if the column is an even number
    --  0 * (number_of_slice_rows - new_row_coordinate - 1) +
    --  1 * new_row_coordinate
    --  this means the coordinate is not modified
    constant modified_new_row_coordinate: integer :=
        new_column_is_odd * (number_of_slice_rows - new_row_coordinate - 1) +
        (1 - new_column_is_odd) * new_row_coordinate;

    constant x_slice: integer := x_base + x_slice_step * new_column_coordinate;
    constant y_2slice: integer :=
        y_base - slice_per_pe_item * modified_new_row_coordinate;

    signal func: std_logic_vector (y'range);

    attribute bel: string;
    attribute loc: string;
    attribute lock_pins: string;
begin
    process (clk)
    begin
        if (clk'event and clk = '1') then
            y <= func;
        end if;
    end process;

    rom_arr: for i in y'range generate

        -- (y'high - i)/y'length -> 0..3 give 1
        --                          4..7 give 0
        -- the result will give the upper or the lower slice
        -- the most significant bits go to the upper slice

        rom_arr_last: if (i = y'high) generate
            attribute bel of rom_inst: label is bel_str(i);
            attribute loc of rom_inst: label is "SLICE_X" &
                integer'image(x_slice) &
                "Y" &
                integer'image(y_2slice-(2*(y'high - i)/y'length));
            attribute lock_pins of rom_inst: label is default_mapping;
        begin
            rom_inst: lut6_l
            generic map (INIT => dummy_pe_lut_arr(OWN_ROW, OWN_COLUMN)(i))
            port map (
                LO => func(i),
                I0 => a(i-1),
                I1 => b(i-1),
                I2 => a(i),
                I3 => b(i),
                I4 => a(y'low),
                I5 => b(y'low)
            );
        end generate;

        rom_arr_first: if (i = y'low) generate
            attribute bel of rom_inst: label is bel_str(i);
            attribute loc of rom_inst: label is "SLICE_X" &
                integer'image(x_slice) &
                "Y" &
                integer'image(y_2slice-(2*(y'high - i)/y'length));
            attribute lock_pins of rom_inst: label is default_mapping;
        begin
            rom_inst: lut6_l
            generic map (INIT => dummy_pe_lut_arr(OWN_ROW, OWN_COLUMN)(i))
            port map (
                LO => func(i),
                I0 => a(y'high),
                I1 => b(y'high),
                I2 => a(i),
                I3 => b(i),
                I4 => a(i+1),
                I5 => b(i+1)
            );
        end generate;

        rom_arr_others: if (i > y'low and i < y'high) generate
            attribute bel of rom_inst: label is bel_str(i);
            attribute loc of rom_inst: label is "SLICE_X" &
                integer'image(x_slice) &
                "Y" &
                integer'image(y_2slice-(2*(y'high - i)/y'length));
            attribute lock_pins of rom_inst: label is default_mapping;
        begin
            rom_inst: lut6_l
            generic map (INIT => dummy_pe_lut_arr(OWN_ROW, OWN_COLUMN)(i))
            port map (
                LO => func(i),
                I0 => a(i-1),
                I1 => b(i-1),
                I2 => a(i),
                I3 => b(i),
                I4 => a(i+1),
                I5 => b(i+1)
            );
        end generate;
    end generate;
end behav_pe_item;
