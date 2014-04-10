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

entity input_pipeline is
    port (
        clk: in std_logic;
        input: in  cgp_input_t;
        output: out input_col_t
    );
end input_pipeline;

architecture behav_input_pipeline of input_pipeline is
    type input_double_col_t is array (0 to (primary_inputs-1),
        0 to (2*columns-1)) of cgp_t;

    signal input_pipeline_arr: input_double_col_t;
begin
    first_col: for i in 0 to (primary_inputs-1) generate
        input_pipeline_arr(i, 0) <= input(i);
    end generate;

    row: for i in 0 to (primary_inputs-1) generate
        col: for j in 1 to (2*columns-1) generate
            process (clk) is
            begin
                if (clk'event and clk = '1') then
                    input_pipeline_arr(i, j) <=
                        input_pipeline_arr(i, j-1);
                end if;
            end process;
        end generate;
    end generate;

    output_row: for i in 0 to (primary_inputs-1) generate
        output_col: for j in 0 to (columns-1) generate
            output(i, j) <= input_pipeline_arr(i, 2*j);
        end generate;
    end generate;
end behav_input_pipeline;
