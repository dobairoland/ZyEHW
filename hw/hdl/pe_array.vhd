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

entity pe_array is
    generic (
        COL: integer:= columns;
        ROW: integer:= rows;
        SLICE_X_BASE: integer := 49;
        SLICE_Y_BASE: integer := 99;
        SLICE_X_STEP: integer := 2;
        SLICE_X_COLUMNS: integer := 2
    );
    port (
        clk: in std_logic;
        input: in  cgp_input_t;
        mux_chromosome: in mux_chromosome_t;
        filtered: out cgp_t
    );
end pe_array;

architecture behav_pe_array of pe_array is

    component pe_item is
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
    end component;

    component pe_mux is
        port (
            clk: in std_logic;
            sel: in mux_sel_t;
            input: in mux_input_t;
            o: out cgp_t
        );
    end component;

    component out_mux is
        port (
            sel: in out_mux_sel_t;
            input: in out_mux_t;
            o: out cgp_t
        );
    end component;

    component input_pipeline is
        port (
            clk: in std_logic;
            input: in  cgp_input_t;
            output: out input_col_t
        );
    end component;

    type pe_array_t is array (0 to (ROW-1), 0 to (COL-1)) of cgp_t;
    type mux_in_arr_t is array (0 to (COL-1)) of mux_input_t;
    constant zero: std_logic:= '0';

    signal input_col: input_col_t;
    signal mux_in_arr: mux_in_arr_t;
    signal mux_a_array, mux_b_array, proc_array: pe_array_t;
    signal out_candidate, out_select: cgp_t;
    signal out_mux_input: out_mux_t;
    signal last_pipelined_input, another_pipelined_input: cgp_t;

    attribute dont_touch: string;
    attribute dont_touch of input_pipeline_i: label is "true";

    attribute loc: string;
begin

    input_pipeline_i: input_pipeline
    port map (
        clk => clk,
        input => input,
        output => input_col
    );

    mux_in_first_colpi: for i in 0 to (primary_inputs-1) generate
        mux_in_arr(0)(i) <= input_col(i, 0);
        mux_in_firstrow: for i in 0 to (ROW-1) generate
            --in the first column there are no proc elements
            --therefore inputs are put there
            mux_in_arr(0)(i+primary_inputs) <= input_col(i, 0);
        end generate;
    end generate;

    mux_in_col: for j in 1 to (COL-1) generate
        mux_in_pi: for i in 0 to (primary_inputs-1) generate
            mux_in_arr(j)(i) <= input_col(i, j);
        end generate;
        mux_in_row: for i in 0 to (ROW-1) generate
            mux_in_arr(j)(i+primary_inputs) <= proc_array(i, j-1);
        end generate;
    end generate;

    pe_row: for i in 0 to (ROW-1) generate
        pe_column: for j in 0 to (COL-1) generate
            pe_a_mux: pe_mux
            port map (
                clk => clk,
                sel => mux_chromosome.a_mux(i, j),
                input => mux_in_arr(j),
                o => mux_a_array(i, j)
            );

            pe_b_mux: pe_mux
            port map (
                clk => clk,
                sel => mux_chromosome.b_mux(i, j),
                input => mux_in_arr(j),
                o => mux_b_array(i, j)
            );

            pe_i: pe_item
            generic map (
                OWN_ROW => i,
                OWN_COLUMN => j,
                SLICE_X_BASE => SLICE_X_BASE,
                SLICE_Y_BASE => SLICE_Y_BASE,
                SLICE_X_STEP => SLICE_X_STEP,
                SLICE_X_COLUMNS => SLICE_X_COLUMNS
            )
            port map (
                clk => clk,
                a => mux_a_array(i, j),
                b => mux_b_array(i, j),
                y => proc_array(i, j)
            );
        end generate;
    end generate;

    -- The columns are filled up with dummy (unused) LUTs with unconnected
    -- inputs and outputs. This will ensure that no functional LUTs will be
    -- placed by the placer there. In consequence, the generated bitstream can
    -- be filled up with 0s and no pregenerated bitstream need to be used.
    dummy_column: for i in 0 to SLICE_X_COLUMNS-1 generate
        dummy_row: for j in SLICE_Y_BASE - slice_per_row + 1 to
        SLICE_Y_BASE - COL*ROW*cgp_t'length/lut_per_slice/SLICE_X_COLUMNS
        generate
            dummy_slice: for k in 0 to lut_per_slice-1 generate
                attribute loc of rom_inst: label is "SLICE_X" &
                    integer'image(SLICE_X_BASE+i*SLICE_X_STEP) &
                    "Y" &
                    integer'image(j);
                attribute dont_touch of rom_inst: label is "true";
            begin
                rom_inst: lut6
                port map (
                    O => open,
                    I0 => zero,
                    I1 => zero,
                    I2 => zero,
                    I3 => zero,
                    I4 => zero,
                    I5 => zero
                );
            end generate;
        end generate;
    end generate;

    out_mux_input_gen: for i in 0 to (ROW-1) generate
        out_mux_input(i) <= proc_array(i, COL-1);
    end generate;

    out_mux_i: out_mux
    port map (
        sel => mux_chromosome.out_select,
        input => out_mux_input,
        o => out_candidate
    );

    out_candidate_select: out_mux
    port map (
        sel => mux_chromosome.filter_switch,
        input => out_mux_input,
        o => out_select
    );

    process(clk)
    begin
        if (clk'event and clk = '1') then
            last_pipelined_input <= input_col(correct_pixel_index, COL-1);
            another_pipelined_input <= last_pipelined_input;
            if (out_select(data_width-1) = '1') then
                filtered <= out_candidate;
            else
                filtered <= another_pipelined_input;
            end if;
        end if;
    end process;
end behav_pe_array;
