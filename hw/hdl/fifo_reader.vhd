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

entity fifo_reader is
    port (
        clk: in std_logic;
        fitness_wr: in std_logic;
        eval_start: in std_logic;
        fifo_rdy: in std_logic;
        fifo_almostfull: in std_logic;
        frame_count: out frame_count_t;
        fifo_rden: out std_logic;
        ram_wren: out std_logic;
        ram_wraddr: out img_addr_t;
        start_en: out std_logic
    );
end fifo_reader;

architecture behav_fifo_reader of fifo_reader is
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
        wait_fifo_rdy,
        first_wait_fifo_almostfull,
        first_transfer,
        wait_fillup,
        transfer,
        frame
    );

    constant zero: std_logic:= '0';

    signal state: state_t:= wait_fifo_rdy;
    signal next_state: state_t;
    signal tmp_fifo_rden: std_logic_vector(1 downto 0):= (others => '0');
    signal tmp_ram_wren: std_logic_vector(3 downto 0):= (others => '0');
    signal tmp_start_en: std_logic_vector(1 downto 0):= (others => '0');
    signal pix_count_en, frame_count_en: std_logic;
    signal pix_count, pix_count_del1, pix_count_del2: img_addr_t;
    signal frames: frame_count_t;
begin
    pixel_counter: counter
    generic map (
        BITS => img_addr_size
    )
    port map (
        clk => clk,
        en => pix_count_en,
        rst => zero,
        count => pix_count
    );

    frame_counter: counter
    generic map (
        BITS => frame_counter_bits
    )
    port map (
        clk => clk,
        en => frame_count_en,
        rst => zero,
        count => frames
    );

    next_state_p: process (state, pix_count, fifo_rdy, fifo_almostfull,
        eval_start)
    begin
        case state is
            when wait_fifo_rdy =>
                if fifo_rdy = '1' then
                    next_state <= first_wait_fifo_almostfull;
                else
                    next_state <= wait_fifo_rdy;
                end if;
            when first_wait_fifo_almostfull =>
                if fifo_almostfull = '1' then
                    next_state <= first_transfer;
                else
                    next_state <= first_wait_fifo_almostfull;
                end if;
            when first_transfer =>
                if pix_count = std_logic_vector(to_unsigned(img_pixels-1,
                        img_addr_size)) then
                    next_state <= wait_fillup;
                else
                    next_state <= first_transfer;
                end if;
            when wait_fillup =>
                if fifo_almostfull = '1' and eval_start = '1' then
                    next_state <= transfer;
                else
                    next_state <= wait_fillup;
                end if;
            when transfer =>
                if pix_count = std_logic_vector(to_unsigned(img_pixels-1,
                        img_addr_size)) then
                    next_state <= frame;
                else
                    next_state <= transfer;
                end if;
            when frame =>
                next_state <= wait_fillup;
        end case;
    end process;

    output_p: process (state)
    begin
        tmp_fifo_rden(0) <= '0';
        tmp_ram_wren(0) <= '0';
        tmp_start_en(0) <= '1';
        pix_count_en <= '0';
        frame_count_en <= '0';

        case state is
            when wait_fifo_rdy =>
                tmp_start_en(0) <= '0';
            when first_wait_fifo_almostfull =>
                tmp_start_en(0) <= '0';
            when first_transfer =>
                tmp_fifo_rden(0) <= '1';
                tmp_ram_wren(0) <= '1';
                tmp_start_en(0) <= '0';
                pix_count_en <= '1';
            when wait_fillup =>
            when transfer =>
                tmp_fifo_rden(0) <= '1';
                tmp_ram_wren(0) <= '1';
                pix_count_en <= '1';
            when frame =>
                frame_count_en <= '1';
        end case;
    end process;

    process (clk)
    begin
        if clk'event and clk = '1' then
            state <= next_state;

            tmp_fifo_rden(1) <= tmp_fifo_rden(0);

            tmp_start_en(1) <= tmp_start_en(0);

            tmp_ram_wren(1) <= tmp_ram_wren(0);
            tmp_ram_wren(2) <= tmp_ram_wren(1);
            tmp_ram_wren(3) <= tmp_ram_wren(2);

            pix_count_del1 <= pix_count;
            pix_count_del2 <= pix_count_del1;
            ram_wraddr <= pix_count_del2;

            if fitness_wr = '1' then
                frame_count <= frames;
            end if;
        end if;
    end process;

    fifo_rden <= tmp_fifo_rden(1);
    ram_wren <= tmp_ram_wren(3);
    start_en <= tmp_start_en(1);
end behav_fifo_reader;
