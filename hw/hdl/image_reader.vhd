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

use work.zyehw_pkg.all;

entity image_reader is
    port (
        clk: in std_logic;
        start: in std_logic;
        start_en: in std_logic;
        eval_started: out std_logic;
        pe_start: out std_logic;
        ram_rden: out std_logic;
        ram_rdaddr: out img_addr_t
    );
end image_reader;

architecture behav_image_reader of image_reader is
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
        wait_start_en,
        send,
        wait_start,
        eval_starts
    );

    constant prefetch_end: integer:= serial_img_t'length;
    constant zero: std_logic:= '0';

    signal state: state_t:= wait_start_en;
    signal next_state: state_t;
    signal addr_count, addr_count_reg: img_addr_t;
    signal addr_count_en: std_logic;
    signal rden: std_logic;
    signal rden_reg: std_logic:= '0';
    signal eval: std_logic;
    signal eval_reg: std_logic:= '0';
begin
    addr_counter: counter
    generic map (
        BITS => img_addr_size
    )
    port map (
        clk => clk,
        en => addr_count_en,
        rst => zero,
        count => addr_count
    );

    next_state_p: process (state, addr_count, start, start_en)
    begin
        case state is
            when wait_start_en =>
                if start_en = '1' then
                    next_state <= send;
                else
                    next_state <= wait_start_en;
                end if;
            when send =>
                if addr_count = std_logic_vector(to_unsigned(prefetch_end,
                        img_addr_size)) then
                    next_state <= wait_start;
                else
                    next_state <= send;
                end if;
            when wait_start =>
                if start = '1' then
                    next_state <= eval_starts;
                else
                    next_state <= wait_start;
                end if;
            when eval_starts =>
                next_state <= send;
        end case;
    end process;

    output_p: process (state)
    begin
        addr_count_en <= '0';
        rden <= '0';
        eval <= '0';

        case state is
            when wait_start_en =>
            when send =>
                addr_count_en <= '1';
                rden <= '1';
            when wait_start =>
            when eval_starts =>
                addr_count_en <= '1';
                rden <= '1';
                eval <= '1';
        end case;
    end process;

    process (clk)
    begin
        if clk'event and clk = '1' then
            state <= next_state;

            addr_count_reg <= addr_count;
            rden_reg <= rden;
            eval_reg <= eval;
        end if;
    end process;

    ram_rdaddr <= addr_count_reg;
    ram_rden <= rden_reg;
    pe_start <= eval_reg;
    eval_started <= eval_reg;
end behav_image_reader;
