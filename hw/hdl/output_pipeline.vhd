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

entity output_pipeline is
    generic (
        DEPTH: integer
    );
    port (
        clk: in std_logic;
        en: in std_logic;
        input: in cgp_t;
        output: out cgp_t
    );
end output_pipeline;

architecture behav_output_pipeline of output_pipeline is
    type pipeline_t is array (0 to DEPTH) of cgp_t;

    signal pipeline: pipeline_t;
begin
    pipeline(pipeline_t'low) <= input;
    output <= pipeline(pipeline_t'high);

    pipe: for i in pipeline_t'low+1 to pipeline_t'high generate
        process (clk)
        begin
            if clk'event and clk = '1' then
                if en = '1' then
                    pipeline(i) <= pipeline(i-1);
                end if;
            end if;
        end process;
    end generate;
end behav_output_pipeline;
