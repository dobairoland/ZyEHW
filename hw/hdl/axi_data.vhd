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

entity axi_data is
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
end axi_data;

architecture beh_axi_data of axi_data is
        -- Speed at 180 MHz: 144 ns / sample (without sample generation)
        -- Speed at 100 MHz: 200 ns / sample (without sample generation)
    constant addr_width: integer:= 32;
    constant data_width: integer:= 32;
    constant w32bytes: integer:= data_width/8;

    signal awready: std_logic;
    signal wready: std_logic;
    signal bvalid: std_logic;
    signal arready: std_logic;
    signal rdata: std_logic_vector(addr_width-1 downto 0);
    signal rvalid: std_logic;

    subtype reg_t is std_logic_vector(data_width-1 downto 0);

    signal databus: std_logic_vector(data_width-1 downto 0);
    signal reg: reg_t;
    signal stat_reg: reg_t;
    signal reg_rden: std_logic;
    signal reg_wren: std_logic;

    signal datawr: std_logic:= '0';
begin
    stat_reg(0) <= near_full;
    stat_reg(reg_t'high downto 1) <= (others => '0');

    dataout_wr <= datawr;
    dataout.input <=  reg(1*cgp_t'length-1 downto 0*cgp_t'length);
    dataout.output <= reg(2*cgp_t'length-1 downto 1*cgp_t'length);

    -- The register is updated when reg_wren = 1 and there is a rising edge on
    -- s_axi_clk. The register value is valid in the next cycle, therefore we
    -- need to delay this signal.
    process (s_axi_aclk) is
    begin
        if rising_edge(s_axi_aclk) then
            datawr <= reg_wren;
        end if;
    end process;

    s_axi_awready <= awready;
    s_axi_wready <= wready;
    s_axi_bvalid <= bvalid;
    s_axi_arready <= arready;
    s_axi_rdata <= rdata;
    s_axi_rvalid <= rvalid;

    reg_wren <= wready and s_axi_wvalid and awready and s_axi_awvalid;
    reg_rden <= arready and s_axi_arvalid and (not rvalid);

    process (s_axi_aclk)
    begin
        if rising_edge(s_axi_aclk) then
            if s_axi_aresetn = '0' then
                awready <= '0';
                wready <= '0';
                bvalid  <= '0';
                arready <= '0';
                rvalid <= '0';
                rdata  <= (others => '0');
            else
                if (awready = '0' and s_axi_awvalid = '1' and
                        s_axi_wvalid = '1') then
                    awready <= '1';
                else
                    awready <= '0';
                end if;

                if (wready = '0' and s_axi_wvalid = '1' and
                        s_axi_awvalid = '1') then
                    wready <= '1';
                else
                    wready <= '0';
                end if;

                if (awready = '1' and s_axi_awvalid = '1' and wready = '1'
                        and s_axi_wvalid = '1' and bvalid = '0') then
                    bvalid <= '1';
                elsif (s_axi_bready = '1' and bvalid = '1') then
                    bvalid <= '0';
                end if;

                if (arready = '0' and s_axi_arvalid = '1') then
                    arready <= '1';
                else
                    arready <= '0';
                end if;

                if (arready = '1' and s_axi_arvalid = '1' and rvalid = '0')
                then
                    rvalid <= '1';
                elsif (rvalid = '1' and s_axi_rready = '1') then
                    rvalid <= '0';
                end if;

                if (reg_rden = '1') then
                    rdata <= databus;
                end if;
            end if;
        end if;
    end process;

    write_process: process (s_axi_aclk) is
    begin
        if rising_edge(s_axi_aclk) then
            if s_axi_aresetn = '0' then
                reg <= (others => '0');
            else
                if (reg_wren = '1')
                then
                    for j in 0 to w32bytes-1 loop
                        if s_axi_wstrb(j) = '1' then
                            reg((j+1)*8-1 downto j*8) <=
                                s_axi_wdata((j+1)*8-1 downto j*8);
                        end if;
                    end loop;
                end if;
            end if;
        end if;
    end process;

    process (stat_reg, s_axi_aresetn)
    begin
        if s_axi_aresetn = '0' then
            databus <= (others => '1');
        else
            databus <= stat_reg;
        end if;
    end process;

end beh_axi_data;
