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

entity tb_ram_rdwr is
end tb_ram_rdwr;

architecture behav_tb_ram_rdwr of tb_ram_rdwr is
    component fifo_reader is
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
    end component;

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

    component image_window is
        port (
            clk: in std_logic;
            en: in std_logic;
            input: in cgp_t;
            redundant_kernel_pixels: out cgp_input_redundant_t
        );
    end component;

    component output_pipeline is
        generic (
            DEPTH: integer
        );
        port (
            clk: in std_logic;
            en: in std_logic;
            input: in cgp_t;
            output: out cgp_t
        );
    end component;

    component image_reader is
        port (
            clk: in std_logic;
            start: in std_logic;
            start_en: in std_logic;
            eval_started: out std_logic;
            pe_start: out std_logic;
            ram_rden: out std_logic;
            ram_rdaddr: out img_addr_t
        );
    end component;

    constant datawidth: integer:= 2*cgp_t'length;

    signal clk: std_logic;
    signal fitness_wr: std_logic;
    signal eval_start: std_logic;
    signal fifo_rdy: std_logic;
    signal fifo_almostfull: std_logic;
    signal frame_count: frame_count_t;
    signal fifo_rden: std_logic;
    signal ram_wren: std_logic;
    signal ram_wraddr: img_addr_t;
    signal start_en: std_logic;
    signal start: std_logic;
    signal pe_start: std_logic;
    signal kernel: cgp_input_redundant_t;
    signal delayed_correct: cgp_t;

    signal di_reg: std_logic_vector(datawidth-1 downto 0):= (others => '0');
    signal do, di, di_del: std_logic_vector(datawidth-1 downto 0);
    signal ram_rden: std_logic;
    signal ram_rdaddr: std_logic_vector(img_addr_size-1 downto 0);
begin
    start <= '0', '1' after 50 us, '0' after 168 us, '1' after 300 us, '0'
        after 350 us, '1' after 600 us;

    fifo_rdy <= '0', '1' after 200 ns;

    fitness_wr <= '0', '1' after 340 us, '0' after 340010 ns, '1' after 500
        us, '0' after 500010 ns;
    fifo_almostfull <= '0', '1' after 300 ns, '0' after 310 ns, '1' after 250
        us, '0' after 332 us;

    di_proc: process (clk)
    begin
        if clk'event and clk = '1' then
            if fifo_rden = '1' then
                di_reg <= std_logic_vector(unsigned(di_reg) + 1);
            end if;

            di_del <= di_reg;
            di <= di_del; -- the response is like from the FIFO
        end if;
    end process;

    fifo_reader_i: fifo_reader
    port map (
        clk => clk,
        fitness_wr => fitness_wr,
        eval_start => eval_start,
        fifo_rdy => fifo_rdy,
        fifo_almostfull => fifo_almostfull,
        frame_count => frame_count,
        fifo_rden => fifo_rden,
        ram_wren => ram_wren,
        ram_wraddr => ram_wraddr,
        start_en => start_en
    );

    ram_i: ram
    generic map (
        AWIDTH => img_addr_size,
        DWIDTH => datawidth
    )
    port map (
        rdclk => clk,
        rden => ram_rden,
        rdaddr => ram_rdaddr,
        do => do,
        wrclk => clk,
        wren => ram_wren,
        wraddr => ram_wraddr,
        di => di
    );

    image_window_i: image_window
    port map (
        clk => clk,
        en => ram_rden,
        input => do(2*cgp_t'length-1 downto cgp_t'length),
        redundant_kernel_pixels => kernel
    );

    output_pipeline_i: output_pipeline
    generic map (
        DEPTH => correct_delay_in_window
    )
    port map (
        clk => clk,
        en => ram_rden,
        input => do(cgp_t'length-1 downto 0),
        output => delayed_correct
    );

    image_reader_i: image_reader
    port map (
        clk => clk,
        start => start,
        start_en => start_en,
        eval_started => eval_start,
        pe_start => pe_start,
        ram_rden => ram_rden,
        ram_rdaddr => ram_rdaddr
    );

    clk_proc: process
        variable tmp_clk: std_logic:= '1';
    begin
        clk <= tmp_clk;
        tmp_clk := not tmp_clk;
        wait for 5 ns;
    end process;
end behav_tb_ram_rdwr;
