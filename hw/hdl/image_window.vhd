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

use work.zyehw_pkg.all;

entity image_window is
    port (
        clk: in std_logic;
        en: in std_logic;
        input: in cgp_t;
        redundant_kernel_pixels: out cgp_input_redundant_t
    );
end image_window;

architecture behav_image_window of image_window is
    signal serial_img: serial_img_t;
    signal tmp_pixels: cgp_input_redundant_t;

    -- The don_touch attribute should be on the signal which is duplicated
    -- into redundant registers. However cannot be placed on the inner
    -- registers because the synthesizer would add weird proxy LUTs.
    -- Therefore, an individual registered temporary signal should be used.
    attribute dont_touch: string;
    attribute dont_touch of tmp_pixels: signal is "true";
begin
    process (clk)
    begin
        if clk'event and clk = '1' then
            if en = '1' then
                serial_img(0) <= input;
                for i in 1 to (serial_img_t'length-1) loop
                    serial_img(i) <= serial_img(i-1);
                end loop;

                -- This will generate the actual redundant registers
                redundant_kernel_pixels <= tmp_pixels;
            end if;
        end if;
    end process;

    -- We generate a temporary signal which is a predecessor of the output
    -- register.
    kernel_row: for i in 0 to (img_kernel-1) generate
        kernel_column: for j in 0 to (img_kernel-1) generate
            constant src_ind: integer:= serial_img_t'length-1-j-i*img_size;
            constant dst_ind: integer:= j+i*img_kernel;
        begin
            kernel_redundant: for k in tmp_pixels'range generate
                redund_from_input: if src_ind = 0 generate
                    tmp_pixels(k)(dst_ind) <= input;
                    -- because previously this was used:
                    -- serial_img(0) <= input;
                end generate;

                other_redundats: if src_ind > 0 generate
                    tmp_pixels(k)(dst_ind) <= serial_img(src_ind - 1);
                    -- minus one because serial_image is generated
                    -- like this: serial_img(i) <= serial_img(i-1)
                end generate;
            end generate;
        end generate;
    end generate;
end behav_image_window;
