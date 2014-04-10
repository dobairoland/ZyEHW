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

entity zyehw is
    port (
        irq_err: inout std_logic;
        DDR_addr: inout std_logic_vector(14 downto 0);
        DDR_ba: inout std_logic_vector(2 downto 0);
        DDR_cas_n: inout std_logic;
        DDR_ck_n: inout std_logic;
        DDR_ck_p: inout std_logic;
        DDR_cke: inout std_logic;
        DDR_cs_n: inout std_logic;
        DDR_dm: inout std_logic_vector(3 downto 0);
        DDR_dq: inout std_logic_vector(31 downto 0);
        DDR_dqs_n: inout std_logic_vector(3 downto 0);
        DDR_dqs_p: inout std_logic_vector(3 downto 0);
        DDR_odt: inout std_logic;
        DDR_ras_n: inout std_logic;
        DDR_reset_n: inout std_logic;
        DDR_we_n: inout std_logic;
        FIXED_IO_ddr_vrn: inout std_logic;
        FIXED_IO_ddr_vrp: inout std_logic;
        FIXED_IO_mio: inout std_logic_vector(53 downto 0);
        FIXED_IO_ps_clk: inout std_logic;
        FIXED_IO_ps_porb: inout std_logic;
        FIXED_IO_ps_srstb: inout std_logic
    );
end zyehw;

architecture struct_zyehw of zyehw is
    component dsgn_wrapper is
        port (
            DDR_cas_n: inout std_logic;
            DDR_cke: inout std_logic;
            DDR_ck_n: inout std_logic;
            DDR_ck_p: inout std_logic;
            DDR_cs_n: inout std_logic;
            DDR_reset_n: inout std_logic;
            DDR_odt: inout std_logic;
            DDR_ras_n: inout std_logic;
            DDR_we_n: inout std_logic;
            DDR_ba: inout std_logic_vector(2 downto 0);
            DDR_addr: inout std_logic_vector(14 downto 0);
            DDR_dm: inout std_logic_vector(3 downto 0);
            DDR_dq: inout std_logic_vector(31 downto 0);
            DDR_dqs_n: inout std_logic_vector(3 downto 0);
            DDR_dqs_p: inout std_logic_vector(3 downto 0);
            FIXED_IO_mio: inout std_logic_vector(53 downto 0);
            FIXED_IO_ddr_vrn: inout std_logic;
            FIXED_IO_ddr_vrp: inout std_logic;
            FIXED_IO_ps_srstb: inout std_logic;
            FIXED_IO_ps_clk: inout std_logic;
            FIXED_IO_ps_porb: inout std_logic;
            ACLK: out std_logic;
            irq_f2p: in std_logic_vector(0 to 0);
            M00_AXI_1_arvalid: out std_logic;
            M00_AXI_1_awvalid: out std_logic;
            M00_AXI_1_bready: out std_logic;
            M00_AXI_1_rready: out std_logic;
            M00_AXI_1_wvalid: out std_logic;
            M00_AXI_1_arprot: out std_logic_vector(2 downto 0);
            M00_AXI_1_awprot: out std_logic_vector(2 downto 0);
            M00_AXI_1_araddr: out std_logic_vector(31 downto 0);
            M00_AXI_1_awaddr: out std_logic_vector(31 downto 0);
            M00_AXI_1_wdata: out std_logic_vector(31 downto 0);
            M00_AXI_1_wstrb: out std_logic_vector(3 downto 0);
            M00_AXI_1_arready: in std_logic;
            M00_AXI_1_awready: in std_logic;
            M00_AXI_1_bvalid: in std_logic;
            M00_AXI_1_rvalid: in std_logic;
            M00_AXI_1_wready: in std_logic;
            M00_AXI_1_bresp: in std_logic_vector(1 downto 0);
            M00_AXI_1_rresp: in std_logic_vector(1 downto 0);
            M00_AXI_1_rdata: in std_logic_vector(31 downto 0);
            M00_AXI_0_arvalid: out std_logic;
            M00_AXI_0_awvalid: out std_logic;
            M00_AXI_0_bready: out std_logic;
            M00_AXI_0_rready: out std_logic;
            M00_AXI_0_wvalid: out std_logic;
            M00_AXI_0_arprot: out std_logic_vector(2 downto 0);
            M00_AXI_0_awprot: out std_logic_vector(2 downto 0);
            M00_AXI_0_araddr: out std_logic_vector(31 downto 0);
            M00_AXI_0_awaddr: out std_logic_vector(31 downto 0);
            M00_AXI_0_wdata: out std_logic_vector(31 downto 0);
            M00_AXI_0_wstrb: out std_logic_vector(3 downto 0);
            M00_AXI_0_arready: in std_logic;
            M00_AXI_0_awready: in std_logic;
            M00_AXI_0_bvalid: in std_logic;
            M00_AXI_0_rvalid: in std_logic;
            M00_AXI_0_wready: in std_logic;
            M00_AXI_0_bresp: in std_logic_vector(1 downto 0);
            M00_AXI_0_rresp: in std_logic_vector(1 downto 0);
            M00_AXI_0_rdata: in std_logic_vector(31 downto 0)
       );
    end component;

    component ehw_core is
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
    end component;

    signal ACLK: std_logic;
    signal M00_AXI_0_araddr: std_logic_vector(31 downto 0);
    signal M00_AXI_0_arprot: std_logic_vector(2 downto 0);
    signal M00_AXI_0_arready: std_logic;
    signal M00_AXI_0_arvalid: std_logic;
    signal M00_AXI_0_awaddr: std_logic_vector(31 downto 0);
    signal M00_AXI_0_awprot: std_logic_vector(2 downto 0);
    signal M00_AXI_0_awready: std_logic;
    signal M00_AXI_0_awvalid: std_logic;
    signal M00_AXI_0_bready: std_logic;
    signal M00_AXI_0_bresp: std_logic_vector(1 downto 0);
    signal M00_AXI_0_bvalid: std_logic;
    signal M00_AXI_0_rdata: std_logic_vector(31 downto 0);
    signal M00_AXI_0_rready: std_logic;
    signal M00_AXI_0_rresp: std_logic_vector(1 downto 0);
    signal M00_AXI_0_rvalid: std_logic;
    signal M00_AXI_0_wdata: std_logic_vector(31 downto 0);
    signal M00_AXI_0_wready: std_logic;
    signal M00_AXI_0_wstrb: std_logic_vector(3 downto 0);
    signal M00_AXI_0_wvalid: std_logic;
    signal M00_AXI_1_araddr: std_logic_vector(31 downto 0);
    signal M00_AXI_1_arprot: std_logic_vector(2 downto 0);
    signal M00_AXI_1_arready: std_logic;
    signal M00_AXI_1_arvalid: std_logic;
    signal M00_AXI_1_awaddr: std_logic_vector(31 downto 0);
    signal M00_AXI_1_awprot: std_logic_vector(2 downto 0);
    signal M00_AXI_1_awready: std_logic;
    signal M00_AXI_1_awvalid: std_logic;
    signal M00_AXI_1_bready: std_logic;
    signal M00_AXI_1_bresp: std_logic_vector(1 downto 0);
    signal M00_AXI_1_bvalid: std_logic;
    signal M00_AXI_1_rdata: std_logic_vector(31 downto 0);
    signal M00_AXI_1_rready: std_logic;
    signal M00_AXI_1_rresp: std_logic_vector(1 downto 0);
    signal M00_AXI_1_rvalid: std_logic;
    signal M00_AXI_1_wdata: std_logic_vector(31 downto 0);
    signal M00_AXI_1_wready: std_logic;
    signal M00_AXI_1_wstrb: std_logic_vector(3 downto 0);
    signal M00_AXI_1_wvalid: std_logic;
    signal irq_f2p: std_logic_vector(0 to 0);
begin

irq_err <= irq_f2p(0);

dsgn_i: dsgn_wrapper
    port map (
        ACLK => ACLK,
        DDR_addr(14 downto 0) => DDR_addr(14 downto 0),
        DDR_ba(2 downto 0) => DDR_ba(2 downto 0),
        DDR_cas_n => DDR_cas_n,
        DDR_ck_n => DDR_ck_n,
        DDR_ck_p => DDR_ck_p,
        DDR_cke => DDR_cke,
        DDR_cs_n => DDR_cs_n,
        DDR_dm(3 downto 0) => DDR_dm(3 downto 0),
        DDR_dq(31 downto 0) => DDR_dq(31 downto 0),
        DDR_dqs_n(3 downto 0) => DDR_dqs_n(3 downto 0),
        DDR_dqs_p(3 downto 0) => DDR_dqs_p(3 downto 0),
        DDR_odt => DDR_odt,
        DDR_ras_n => DDR_ras_n,
        DDR_reset_n => DDR_reset_n,
        DDR_we_n => DDR_we_n,
        FIXED_IO_ddr_vrn => FIXED_IO_ddr_vrn,
        FIXED_IO_ddr_vrp => FIXED_IO_ddr_vrp,
        FIXED_IO_mio(53 downto 0) => FIXED_IO_mio(53 downto 0),
        FIXED_IO_ps_clk => FIXED_IO_ps_clk,
        FIXED_IO_ps_porb => FIXED_IO_ps_porb,
        FIXED_IO_ps_srstb => FIXED_IO_ps_srstb,
        M00_AXI_0_araddr(31 downto 0) => M00_AXI_0_araddr(31 downto 0),
        M00_AXI_0_arprot(2 downto 0) => M00_AXI_0_arprot(2 downto 0),
        M00_AXI_0_arready => M00_AXI_0_arready,
        M00_AXI_0_arvalid => M00_AXI_0_arvalid,
        M00_AXI_0_awaddr(31 downto 0) => M00_AXI_0_awaddr(31 downto 0),
        M00_AXI_0_awprot(2 downto 0) => M00_AXI_0_awprot(2 downto 0),
        M00_AXI_0_awready => M00_AXI_0_awready,
        M00_AXI_0_awvalid => M00_AXI_0_awvalid,
        M00_AXI_0_bready => M00_AXI_0_bready,
        M00_AXI_0_bresp(1 downto 0) => M00_AXI_0_bresp(1 downto 0),
        M00_AXI_0_bvalid => M00_AXI_0_bvalid,
        M00_AXI_0_rdata(31 downto 0) => M00_AXI_0_rdata(31 downto 0),
        M00_AXI_0_rready => M00_AXI_0_rready,
        M00_AXI_0_rresp(1 downto 0) => M00_AXI_0_rresp(1 downto 0),
        M00_AXI_0_rvalid => M00_AXI_0_rvalid,
        M00_AXI_0_wdata(31 downto 0) => M00_AXI_0_wdata(31 downto 0),
        M00_AXI_0_wready => M00_AXI_0_wready,
        M00_AXI_0_wstrb(3 downto 0) => M00_AXI_0_wstrb(3 downto 0),
        M00_AXI_0_wvalid => M00_AXI_0_wvalid,
        M00_AXI_1_araddr(31 downto 0) => M00_AXI_1_araddr(31 downto 0),
        M00_AXI_1_arprot(2 downto 0) => M00_AXI_1_arprot(2 downto 0),
        M00_AXI_1_arready => M00_AXI_1_arready,
        M00_AXI_1_arvalid => M00_AXI_1_arvalid,
        M00_AXI_1_awaddr(31 downto 0) => M00_AXI_1_awaddr(31 downto 0),
        M00_AXI_1_awprot(2 downto 0) => M00_AXI_1_awprot(2 downto 0),
        M00_AXI_1_awready => M00_AXI_1_awready,
        M00_AXI_1_awvalid => M00_AXI_1_awvalid,
        M00_AXI_1_bready => M00_AXI_1_bready,
        M00_AXI_1_bresp(1 downto 0) => M00_AXI_1_bresp(1 downto 0),
        M00_AXI_1_bvalid => M00_AXI_1_bvalid,
        M00_AXI_1_rdata(31 downto 0) => M00_AXI_1_rdata(31 downto 0),
        M00_AXI_1_rready => M00_AXI_1_rready,
        M00_AXI_1_rresp(1 downto 0) => M00_AXI_1_rresp(1 downto 0),
        M00_AXI_1_rvalid => M00_AXI_1_rvalid,
        M00_AXI_1_wdata(31 downto 0) => M00_AXI_1_wdata(31 downto 0),
        M00_AXI_1_wready => M00_AXI_1_wready,
        M00_AXI_1_wstrb(3 downto 0) => M00_AXI_1_wstrb(3 downto 0),
        M00_AXI_1_wvalid => M00_AXI_1_wvalid,
        irq_f2p(0) => irq_f2p(0)
    );

    ehw_core_i: ehw_core
    port map (
                 clk => ACLK,
                 irq_f2p => irq_f2p,
                 M00_AXI_1_arvalid => M00_AXI_1_arvalid,
                 M00_AXI_1_awvalid => M00_AXI_1_awvalid,
                 M00_AXI_1_bready => M00_AXI_1_bready,
                 M00_AXI_1_rready => M00_AXI_1_rready,
                 M00_AXI_1_wvalid => M00_AXI_1_wvalid,
                 M00_AXI_1_arprot => M00_AXI_1_arprot,
                 M00_AXI_1_awprot => M00_AXI_1_awprot,
                 M00_AXI_1_araddr => M00_AXI_1_araddr,
                 M00_AXI_1_awaddr => M00_AXI_1_awaddr,
                 M00_AXI_1_wdata => M00_AXI_1_wdata,
                 M00_AXI_1_wstrb => M00_AXI_1_wstrb,
                 M00_AXI_1_arready => M00_AXI_1_arready,
                 M00_AXI_1_awready => M00_AXI_1_awready,
                 M00_AXI_1_bvalid => M00_AXI_1_bvalid,
                 M00_AXI_1_rvalid => M00_AXI_1_rvalid,
                 M00_AXI_1_wready => M00_AXI_1_wready,
                 M00_AXI_1_bresp => M00_AXI_1_bresp,
                 M00_AXI_1_rresp => M00_AXI_1_rresp,
                 M00_AXI_1_rdata => M00_AXI_1_rdata,
                 M00_AXI_0_arvalid => M00_AXI_0_arvalid,
                 M00_AXI_0_awvalid => M00_AXI_0_awvalid,
                 M00_AXI_0_bready => M00_AXI_0_bready,
                 M00_AXI_0_rready => M00_AXI_0_rready,
                 M00_AXI_0_wvalid => M00_AXI_0_wvalid,
                 M00_AXI_0_arprot => M00_AXI_0_arprot,
                 M00_AXI_0_awprot => M00_AXI_0_awprot,
                 M00_AXI_0_araddr => M00_AXI_0_araddr,
                 M00_AXI_0_awaddr => M00_AXI_0_awaddr,
                 M00_AXI_0_wdata => M00_AXI_0_wdata,
                 M00_AXI_0_wstrb => M00_AXI_0_wstrb,
                 M00_AXI_0_arready => M00_AXI_0_arready,
                 M00_AXI_0_awready => M00_AXI_0_awready,
                 M00_AXI_0_bvalid => M00_AXI_0_bvalid,
                 M00_AXI_0_rvalid => M00_AXI_0_rvalid,
                 M00_AXI_0_wready => M00_AXI_0_wready,
                 M00_AXI_0_bresp => M00_AXI_0_bresp,
                 M00_AXI_0_rresp => M00_AXI_0_rresp,
                 M00_AXI_0_rdata => M00_AXI_0_rdata
    );
end struct_zyehw;
