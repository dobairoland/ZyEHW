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

entity fitness_controler is
    port (
        clk: in std_logic;
        pe_start: in std_logic;
        fitness_wr: out std_logic;
        fitness_en: out std_logic;
        fitness_rst: out std_logic
    );
end fitness_controler;

architecture behav_fitness_controler of fitness_controler is
    component counter is
        generic (
            BITS: integer
        );
        port (
            clk: in std_logic;
            en: in std_logic;
            rst: in std_logic;
            count: out std_logic_vector(BITS-1 downto 0)
        );
    end component;

    type state_t is (
        wait_start,
        wait_pe_array_delay,
        process_row,
        skip_to_next_row,
        next_row,
        complete_fitness,
        write_fitness,
        back_to_beginning
    );

    constant zero: std_logic:= '0';
    constant fitness_delay: integer:= 4 - 1;
    constant pe_delay: integer:= 2*columns-1;
    constant pe_delay_bits: integer:= integer(ceil(log2(real(pe_delay))));
    constant img_edge_bits: integer:= integer(ceil(log2(real(img_size))));
    constant fitness_delay_bits: integer:=
        integer(ceil(log2(real(fitness_delay))));
    constant next_row_skip: integer:= img_kernel - 1;
    constant int_columns: integer:= (img_size - 1) - next_row_skip;
    constant skip_columns: integer:= (img_size - 1)
                        - 1; -- there is an increment in the next cycle too
    constant int_rows: integer:= int_columns;

    signal state: state_t:= wait_start;
    signal next_state: state_t;

    signal tmp_fitness_wr: std_logic;
    signal tmp_fitness_en: std_logic;
    signal tmp_fitness_rst: std_logic;
    signal fitness_wr_reg: std_logic:= '0';
    signal fitness_en_reg: std_logic:= '0';
    signal fitness_rst_reg: std_logic:= '0';

    signal pe_delay_count_en, pe_delay_count_rst: std_logic;
    signal pe_delay_count: std_logic_vector(pe_delay_bits-1 downto 0);

    signal column_count_en: std_logic;
    signal column_count: std_logic_vector(img_edge_bits-1 downto 0);

    signal row_count_en: std_logic;
    signal row_count: std_logic_vector(img_edge_bits-1 downto 0);

    signal fitness_delay_count_en, fitness_delay_count_rst: std_logic;
    signal fitness_delay_count: std_logic_vector(fitness_delay_bits-1 downto
        0);
begin
    pe_delay_counter: counter
    generic map (
        BITS => pe_delay_bits
    )
    port map (
        clk => clk,
        en => pe_delay_count_en,
        rst => pe_delay_count_rst,
        count => pe_delay_count
    );

    column_counter: counter
    generic map (
        BITS => img_edge_bits
    )
    port map (
        clk => clk,
        en => column_count_en,
        rst => zero,
        count => column_count
    );

    row_counter: counter
    generic map (
        BITS => img_edge_bits
    )
    port map (
        clk => clk,
        en => row_count_en,
        rst => zero,
        count => row_count
    );

    fitness_delay_counter: counter
    generic map (
        BITS => fitness_delay_bits
    )
    port map (
        clk => clk,
        en => fitness_delay_count_en,
        rst => fitness_delay_count_rst,
        count => fitness_delay_count
    );

    next_state_p: process (state, pe_delay_count, column_count, row_count,
        fitness_delay_count, pe_start)
    begin
        case state is
            when wait_start =>
                if pe_start = '1' then
                    next_state <= wait_pe_array_delay;
                else
                    next_state <= wait_start;
                end if;
            when wait_pe_array_delay =>
                if pe_delay_count = std_logic_vector(to_unsigned(pe_delay,
                        pe_delay_bits)) then
                    next_state <= process_row;
                else
                    next_state <= wait_pe_array_delay;
                end if;
            when process_row =>
                if column_count = std_logic_vector(to_unsigned(int_columns,
                        img_edge_bits)) then
                    next_state <= skip_to_next_row;
                else
                    next_state <= process_row;
                end if;
            when skip_to_next_row =>
                if column_count = std_logic_vector(to_unsigned(skip_columns,
                        img_edge_bits)) then
                    next_state <= next_row;
                else
                    next_state <= skip_to_next_row;
                end if;
            when next_row =>
                if row_count = std_logic_vector(to_unsigned(int_rows,
                        img_edge_bits)) then
                    next_state <= complete_fitness;
                else
                    next_state <= process_row;
                end if;
            when complete_fitness =>
                if fitness_delay_count =
                        std_logic_vector(to_unsigned(fitness_delay-1,
                        fitness_delay_bits)) then
                    next_state <= write_fitness;
                else
                    next_state <= complete_fitness;
                end if;
            when write_fitness =>
                    next_state <= back_to_beginning;
            when back_to_beginning =>
                if row_count = std_logic_vector(to_unsigned(img_size-1,
                        img_edge_bits)) then
                    next_state <= wait_start;
                else
                    next_state <= back_to_beginning;
                end if;
        end case;
    end process;

    output_p: process (state)
    begin
        tmp_fitness_wr <= '0';
        tmp_fitness_en <= '0';
        tmp_fitness_rst <= '0';
        pe_delay_count_en <= '0';
        pe_delay_count_rst <= '0';
        column_count_en <= '0';
        row_count_en <= '0';
        fitness_delay_count_en <= '0';
        fitness_delay_count_rst <= '0';

        case state is
            when wait_start =>
                tmp_fitness_rst <= '1';
                pe_delay_count_rst <= '1';
            when wait_pe_array_delay =>
                pe_delay_count_en <= '1';
            when process_row =>
                column_count_en <= '1';
                tmp_fitness_en <= '1';
            when skip_to_next_row =>
                column_count_en <= '1';
            when next_row =>
                column_count_en <= '1';
                row_count_en <= '1';
            when complete_fitness =>
                tmp_fitness_en <= '1';
                fitness_delay_count_en <= '1';
            when write_fitness =>
                tmp_fitness_wr <= '1';
                fitness_delay_count_rst <= '1';
            when back_to_beginning =>
                row_count_en <= '1';
        end case;
    end process;

    process (clk)
    begin
        if clk'event and clk = '1' then
            state <= next_state;

            fitness_wr_reg <= tmp_fitness_wr;
            fitness_en_reg <= tmp_fitness_en;
            fitness_rst_reg <= tmp_fitness_rst;
        end if;
    end process;

    fitness_wr <= fitness_wr_reg;
    fitness_en <= fitness_en_reg;
    fitness_rst <= fitness_rst_reg;
end behav_fitness_controler;
