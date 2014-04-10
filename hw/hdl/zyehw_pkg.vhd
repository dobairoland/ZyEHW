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

package zyehw_pkg is
    constant columns: integer:= 8;
    constant rows: integer:= 4;
    constant data_width: integer:= 8;
    constant primary_inputs: integer:= 9;
    constant input_sel_width: integer:= 4;
    constant output_sel_width: integer:= 2;
    constant correct_pixel_index: integer:= 4;
    constant core_frequency: integer:= 300;
    constant img_size: integer:= 128;
    constant img_kernel: integer:= 3;
    constant img_pixels: integer:= img_size * img_size;
    constant img_addr_size: integer:= integer(ceil(log2(real(img_pixels))));
    constant fitness_size: integer:= integer(ceil(log2(real(
        ((img_size-img_kernel+1)**2) * (2**data_width)
        -- the number of pixels checked does not contain the border;
        -- (2**data_width) - the biggest possible difference between two
        -- pixels
    ))));
    constant population: integer:= 6;
    constant frame_counter_bits: integer:= 28;
    constant slice_per_row: integer:= 50;
    constant lut_per_slice: integer:= 4;
    constant correct_delay_in_window: integer:= img_size+img_kernel-1;

    subtype cgp_t is std_logic_vector((data_width-1) downto 0);
    type mux_input_t is array(0 to (primary_inputs-1+rows)) of cgp_t;
    subtype mux_sel_t is std_logic_vector((input_sel_width-1) downto 0);
    type cgp_input_t is array(0 to (primary_inputs-1)) of cgp_t;
    type input_col_t is array(0 to (primary_inputs-1), 0 to (columns-1)) of
        cgp_t;
    subtype out_mux_sel_t is std_logic_vector((output_sel_width-1) downto 0);
    type out_mux_t is array(0 to (rows-1)) of cgp_t;
    type mux_arr_t is array(0 to (rows-1), 0 to (columns-1)) of mux_sel_t;
    subtype img_addr_t is std_logic_vector((img_addr_size-1) downto 0);
    subtype fitness_t is std_logic_vector((fitness_size-1) downto 0);
    type fitness_arr_t is array(0 to population-1) of fitness_t;
    subtype frame_count_t is std_logic_vector(frame_counter_bits-1 downto 0);
    type serial_img_t is array (0 to ((img_kernel-1)*img_size+img_kernel-1))
        of cgp_t;
    type cgp_input_redundant_t is array(0 to population-1) of cgp_input_t;

    type fifo_t is record
        input: cgp_t;
        output: cgp_t;
    end record;

    type mux_chromosome_t is record
        a_mux: mux_arr_t;
        b_mux: mux_arr_t;
        out_select: out_mux_sel_t;
        filter_switch: out_mux_sel_t;
    end record;

    type mux_chromosome_arr_t is array(0 to population-1) of mux_chromosome_t;
end;

package body zyehw_pkg is
end package body;
