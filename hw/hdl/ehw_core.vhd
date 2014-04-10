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

entity ehw_core is
    port (
             clk: in std_logic;
             irq_f2p: out std_logic_vector(0 to 0);
             M00_AXI_1_arvalid: in std_logic;
             M00_AXI_1_awvalid: in std_logic;
             M00_AXI_1_bready: in std_logic;
             M00_AXI_1_rready: in std_logic;
             M00_AXI_1_wvalid: in std_logic;
             M00_AXI_1_arprot: in std_logic_vector(2 downto 0);
             M00_AXI_1_awprot: in std_logic_vector(2 downto 0);
             M00_AXI_1_araddr: in std_logic_vector(31 downto 0);
             M00_AXI_1_awaddr: in std_logic_vector(31 downto 0);
             M00_AXI_1_wdata: in std_logic_vector(31 downto 0);
             M00_AXI_1_wstrb: in std_logic_vector(3 downto 0);
             M00_AXI_1_arready: out std_logic;
             M00_AXI_1_awready: out std_logic;
             M00_AXI_1_bvalid: out std_logic;
             M00_AXI_1_rvalid: out std_logic;
             M00_AXI_1_wready: out std_logic;
             M00_AXI_1_bresp: out std_logic_vector(1 downto 0);
             M00_AXI_1_rresp: out std_logic_vector(1 downto 0);
             M00_AXI_1_rdata: out std_logic_vector(31 downto 0);
             M00_AXI_0_arvalid: in std_logic;
             M00_AXI_0_awvalid: in std_logic;
             M00_AXI_0_bready: in std_logic;
             M00_AXI_0_rready: in std_logic;
             M00_AXI_0_wvalid: in std_logic;
             M00_AXI_0_arprot: in std_logic_vector(2 downto 0);
             M00_AXI_0_awprot: in std_logic_vector(2 downto 0);
             M00_AXI_0_araddr: in std_logic_vector(31 downto 0);
             M00_AXI_0_awaddr: in std_logic_vector(31 downto 0);
             M00_AXI_0_wdata: in std_logic_vector(31 downto 0);
             M00_AXI_0_wstrb: in std_logic_vector(3 downto 0);
             M00_AXI_0_arready: out std_logic;
             M00_AXI_0_awready: out std_logic;
             M00_AXI_0_bvalid: out std_logic;
             M00_AXI_0_rvalid: out std_logic;
             M00_AXI_0_wready: out std_logic;
             M00_AXI_0_bresp: out std_logic_vector(1 downto 0);
             M00_AXI_0_rresp: out std_logic_vector(1 downto 0);
             M00_AXI_0_rdata: out std_logic_vector(31 downto 0)
    );
end ehw_core;

architecture struct_ehw_core of ehw_core is

    component axi_evol is
        port (
                 s_axi_aclk: in std_logic;
                 s_axi_aresetn: in std_logic;
                 s_axi_arvalid: in std_logic;
                 s_axi_awvalid: in std_logic;
                 s_axi_bready: in std_logic;
                 s_axi_rready: in std_logic;
                 s_axi_wvalid: in std_logic;
                 s_axi_arprot: in std_logic_vector(2 downto 0);
                 s_axi_awprot: in std_logic_vector(2 downto 0);
                 s_axi_araddr: in std_logic_vector(31 downto 0);
                 s_axi_awaddr: in std_logic_vector(31 downto 0);
                 s_axi_wdata: in std_logic_vector(31 downto 0);
                 s_axi_wstrb: in std_logic_vector(3 downto 0);
                 s_axi_arready: out std_logic;
                 s_axi_awready: out std_logic;
                 s_axi_bvalid: out std_logic;
                 s_axi_rvalid: out std_logic;
                 s_axi_wready: out std_logic;
                 s_axi_bresp: out std_logic_vector(1 downto 0);
                 s_axi_rresp: out std_logic_vector(1 downto 0);
                 s_axi_rdata: out std_logic_vector(31 downto 0);
                 end_flag: in std_logic;
                 start_rst: in std_logic;
                 fitness: in fitness_arr_t;
                 fifo_rderr: in std_logic;
                 fifo_wrerr: in std_logic;
                 frame_count: in frame_count_t;
                 start_flag: out std_logic;
                 chromosome: out mux_chromosome_arr_t
        );
    end component;

    component axi_data is
        port (
                 s_axi_aclk: in std_logic;
                 s_axi_aresetn: in std_logic;
                 s_axi_arvalid: in std_logic;
                 s_axi_awvalid: in std_logic;
                 s_axi_bready: in std_logic;
                 s_axi_rready: in std_logic;
                 s_axi_wvalid: in std_logic;
                 s_axi_arprot: in std_logic_vector(2 downto 0);
                 s_axi_awprot: in std_logic_vector(2 downto 0);
                 s_axi_araddr: in std_logic_vector(31 downto 0);
                 s_axi_awaddr: in std_logic_vector(31 downto 0);
                 s_axi_wdata: in std_logic_vector(31 downto 0);
                 s_axi_wstrb: in std_logic_vector(3 downto 0);
                 s_axi_arready: out std_logic;
                 s_axi_awready: out std_logic;
                 s_axi_bvalid: out std_logic;
                 s_axi_rvalid: out std_logic;
                 s_axi_wready: out std_logic;
                 s_axi_bresp: out std_logic_vector(1 downto 0);
                 s_axi_rresp: out std_logic_vector(1 downto 0);
                 s_axi_rdata: out std_logic_vector(31 downto 0);
                 near_full: in std_logic;
                 dataout_wr: out std_logic;
                 dataout: out fifo_t
        );
    end component;

    component cgp_sync is
        port (
                 cgp_clk: in std_logic;
                 axi_clk: in std_logic;
                 axi_start: in std_logic;
                 axi_mux_chromosome_arr: in mux_chromosome_arr_t;
                 cgp_fifo_almostfull: in std_logic;
                 cgp_fifo_ready: in std_logic;
                 cgp_data: in fifo_t;
                 cgp_datard: out std_logic;
                 axi_end: out std_logic;
                 axi_start_rst: out std_logic;
                 axi_frame_count: out frame_count_t;
                 axi_fitness_arr: out fitness_arr_t
        );
    end component;

    component fifo_sync is
        port (
                 axi0_clk: in std_logic;
                 axi1_clk: in std_logic;
                 cgp_clk: in std_logic;
                 axi0_data: in fifo_t;
                 axi0_datawr: in std_logic;
                 cgp_datard: in std_logic;
                 cgp_data: out fifo_t;
                 cgp_fifo_ready: out std_logic;
                 cgp_half_full: out std_logic;
                 axi0_near_full: out std_logic;
                 axi1_fifo_rderr: out std_logic;
                 axi1_fifo_wrerr: out std_logic
        );
    end component;

    subtype clock_divisor_t is real range 1.000 to 128.000;
    subtype clock_multiply_t is real range 2.000 to 64.000;

    constant clock_multiply: clock_multiply_t := 10.0;
    constant input_frequency: real := 100.0;
    constant input_period_ns_to_ps: real:= 1000.0/input_frequency;
    constant clock_divisor: clock_divisor_t := real(
        clock_multiply*input_frequency)/real(core_frequency);
    constant zero: std_logic:= '0';
    constant one: std_logic:= '1';

    signal mmcm_clk, buf_clk, clk_feed_mmcm_out, clk_feed_mmcm_in: std_logic;
    signal cgp_clk: std_logic;
    signal axi_end: std_logic;
    signal axi_start_rst: std_logic;
    signal axi_fitness_arr: fitness_arr_t;
    signal axi_start: std_logic;
    signal axi_mux_chromosome_arr: mux_chromosome_arr_t;
    signal axi_fifo_rderr, axi_fifo_wrerr: std_logic;
    signal axi0_data, cgp_data: fifo_t;
    signal axi0_datawr, cgp_datard, cgp_fifo_almostfull: std_logic;
    signal cgp_fifo_ready: std_logic;
    signal axi_frame_count: frame_count_t;
    signal axi0_near_full: std_logic;
begin

    axi_data_i: axi_data
    port map (
                 s_axi_aclk => buf_clk,
                 s_axi_aresetn => one,
                 s_axi_arvalid => M00_AXI_0_arvalid,
                 s_axi_awvalid => M00_AXI_0_awvalid,
                 s_axi_bready => M00_AXI_0_bready,
                 s_axi_rready => M00_AXI_0_rready,
                 s_axi_wvalid => M00_AXI_0_wvalid,
                 s_axi_arprot => M00_AXI_0_arprot,
                 s_axi_awprot => M00_AXI_0_awprot,
                 s_axi_araddr => M00_AXI_0_araddr,
                 s_axi_awaddr => M00_AXI_0_awaddr,
                 s_axi_wdata => M00_AXI_0_wdata,
                 s_axi_wstrb => M00_AXI_0_wstrb,
                 s_axi_arready => M00_AXI_0_arready,
                 s_axi_awready => M00_AXI_0_awready,
                 s_axi_bvalid => M00_AXI_0_bvalid,
                 s_axi_rvalid => M00_AXI_0_rvalid,
                 s_axi_wready => M00_AXI_0_wready,
                 s_axi_bresp => M00_AXI_0_bresp,
                 s_axi_rresp => M00_AXI_0_rresp,
                 s_axi_rdata => M00_AXI_0_rdata,
                 near_full => axi0_near_full,
                 dataout_wr => axi0_datawr,
                 dataout => axi0_data
    );

    axi_evol_i: axi_evol
    port map (
                 s_axi_aclk => buf_clk,
                 s_axi_aresetn => one,
                 s_axi_arvalid => M00_AXI_1_arvalid,
                 s_axi_awvalid => M00_AXI_1_awvalid,
                 s_axi_bready => M00_AXI_1_bready,
                 s_axi_rready => M00_AXI_1_rready,
                 s_axi_wvalid => M00_AXI_1_wvalid,
                 s_axi_arprot => M00_AXI_1_arprot,
                 s_axi_awprot => M00_AXI_1_awprot,
                 s_axi_araddr => M00_AXI_1_araddr,
                 s_axi_awaddr => M00_AXI_1_awaddr,
                 s_axi_wdata => M00_AXI_1_wdata,
                 s_axi_wstrb => M00_AXI_1_wstrb,
                 s_axi_arready => M00_AXI_1_arready,
                 s_axi_awready => M00_AXI_1_awready,
                 s_axi_bvalid => M00_AXI_1_bvalid,
                 s_axi_rvalid => M00_AXI_1_rvalid,
                 s_axi_wready => M00_AXI_1_wready,
                 s_axi_bresp => M00_AXI_1_bresp,
                 s_axi_rresp => M00_AXI_1_rresp,
                 s_axi_rdata => M00_AXI_1_rdata,
                 end_flag => axi_end,
                 start_rst => axi_start_rst,
                 fitness => axi_fitness_arr,
                 fifo_rderr => axi_fifo_rderr,
                 fifo_wrerr => axi_fifo_wrerr,
                 frame_count => axi_frame_count,
                 start_flag => axi_start,
                 chromosome => axi_mux_chromosome_arr
    );

    cgp_sync_i: cgp_sync
    port map (
             cgp_clk => cgp_clk,
             axi_clk => buf_clk,
             axi_start => axi_start,
             axi_mux_chromosome_arr => axi_mux_chromosome_arr,
             cgp_fifo_almostfull => cgp_fifo_almostfull,
             cgp_fifo_ready => cgp_fifo_ready,
             cgp_data => cgp_data,
             cgp_datard => cgp_datard,
             axi_end => axi_end,
             axi_start_rst => axi_start_rst,
             axi_frame_count => axi_frame_count,
             axi_fitness_arr => axi_fitness_arr
    );

    mmcm_i : mmcme2_base
    generic map (
                 -- Jitter programming (OPTIMIZED, HIGH, LOW)
                 BANDWIDTH => "OPTIMIZED",
                 -- Multiply value for all CLKOUT (2.000-64.000).
                 CLKFBOUT_MULT_F => clock_multiply,
                 -- Phase offset in degrees of CLKFB (-360.000-360.000).
                 CLKFBOUT_PHASE => 0.0,
                 -- Input clock period in ns to ps resolution (i.e. 33.333 is
                 -- 30 MHz).
                 CLKIN1_PERIOD => input_period_ns_to_ps,
                 -- Divide amount for each CLKOUT (1-128)
                 CLKOUT1_DIVIDE => 1,
                 CLKOUT2_DIVIDE => 1,
                 CLKOUT3_DIVIDE => 1,
                 CLKOUT4_DIVIDE => 1,
                 CLKOUT5_DIVIDE => 1,
                 CLKOUT6_DIVIDE => 1,
                 -- Divide amount for CLKOUT0 (1.000-128.000).
                 CLKOUT0_DIVIDE_F => clock_divisor,
                 -- Duty cycle for each CLKOUT (0.01-0.99).
                 CLKOUT0_DUTY_CYCLE => 0.5,
                 CLKOUT1_DUTY_CYCLE => 0.5,
                 CLKOUT2_DUTY_CYCLE => 0.5,
                 CLKOUT3_DUTY_CYCLE => 0.5,
                 CLKOUT4_DUTY_CYCLE => 0.5,
                 CLKOUT5_DUTY_CYCLE => 0.5,
                 CLKOUT6_DUTY_CYCLE => 0.5,
                 -- Phase offset for each CLKOUT (-360.000-360.000).
                 CLKOUT0_PHASE => 0.0,
                 CLKOUT1_PHASE => 0.0,
                 CLKOUT2_PHASE => 0.0,
                 CLKOUT3_PHASE => 0.0,
                 CLKOUT4_PHASE => 0.0,
                 CLKOUT5_PHASE => 0.0,
                 CLKOUT6_PHASE => 0.0,
                 -- Cascade CLKOUT4 counter with CLKOUT6 (FALSE, TRUE)
                 CLKOUT4_CASCADE => FALSE,
                 DIVCLK_DIVIDE => 1, -- Master division value (1-106)
                 -- Reference input jitter in UI (0.000-0.999).
                 REF_JITTER1 => 0.0,
                 -- Delays DONE until MMCM is locked (FALSE, TRUE)
                 STARTUP_WAIT => FALSE
                )
    port map (
                 CLKOUT0 => mmcm_clk,
                 CLKOUT0B => open,
                 CLKOUT1 => open,
                 CLKOUT1B => open,
                 CLKOUT2 => open,
                 CLKOUT2B => open,
                 CLKOUT3 => open,
                 CLKOUT3B => open,
                 CLKOUT4 => open,
                 CLKOUT5 => open,
                 CLKOUT6 => open,
                 CLKFBOUT => clk_feed_mmcm_out,
                 CLKFBOUTB => open,
                 LOCKED => open,
                 CLKIN1 => buf_clk,
                 PWRDWN => zero,
                 RST => zero,
                 CLKFBIN => clk_feed_mmcm_in
             );

    clock_buff: bufg
    port map (
                 O => cgp_clk,
                 I => mmcm_clk
             );

    feedback_buff: bufg
    port map (
                 O => clk_feed_mmcm_in,
                 I => clk_feed_mmcm_out
             );

    input_clk_buff: bufg
    port map (
                 O => buf_clk,
                 I => clk
             );

    irq_f2p(0) <= axi_fifo_rderr or axi_fifo_wrerr;

    fifo_sync_i: fifo_sync
    port map (
             axi0_clk => buf_clk,
             axi1_clk => buf_clk,
             cgp_clk => cgp_clk,
             axi0_data => axi0_data,
             axi0_datawr => axi0_datawr,
             cgp_datard => cgp_datard,
             cgp_data => cgp_data,
             cgp_fifo_ready => cgp_fifo_ready,
             cgp_half_full => cgp_fifo_almostfull,
             axi0_near_full => axi0_near_full,
             axi1_fifo_rderr => axi_fifo_rderr,
             axi1_fifo_wrerr => axi_fifo_wrerr
    );
end struct_ehw_core;
