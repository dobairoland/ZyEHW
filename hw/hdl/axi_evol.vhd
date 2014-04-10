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

entity axi_evol is
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
end axi_evol;

architecture beh_axi_evol of axi_evol is
    constant addr_width: integer:= 32;
    constant data_width: integer:= 32;
    constant numregisters: integer:= 1              -- control/status
                                    + population    -- fitness
                                    + population *( -- chromosome
                                    2*(             -- a and b inputs
                                    columns*rows    -- PEs
                                    *input_sel_width/data_width
                                    )
                                    + 1             -- out multiplexers
                                );
    constant w32bytes: integer:= data_width/8;
    constant word_bits: integer:= integer(ceil(log2(real(w32bytes))));
    constant reg_bits: integer:= integer(ceil(log2(real(numregisters))));

    signal writeaddr, readaddr: std_logic_vector(reg_bits-1 downto 0);
    signal awready: std_logic;
    signal wready: std_logic;
    signal bvalid: std_logic;
    signal arready: std_logic;
    signal rdata: std_logic_vector(addr_width-1 downto 0);
    signal rvalid: std_logic;

    subtype reg_t is std_logic_vector(data_width-1 downto 0);
    type regs_t is array (0 to numregisters-1) of reg_t;

    signal databus: std_logic_vector(data_width-1 downto 0);
    signal regs: regs_t;
    signal reg_rden: std_logic;
    signal reg_wren: std_logic;
begin

    -- Only the start_flag and the chromosome are real registers here.
    -- The end_flag, fifo_rderr, fifo_wrerr are read together with start_flag
    -- and are read-only. The fitness is also read-only.

    -- Addressing
    -- mux bits:
    -- (31..28) (27..24) (23..20) (19..16) (15..12) (11..8) (7..4) (3..0)
    --
    -- regs(0) - (3) fifo_wrerr, (2) fifo_rderr, (1) end_flag, (0) start_flag
    -- Individual 0:
    -- regs( 1) - b_mux(3, 0), a_mux(3, 0), ..., b_mux(0, 0), a_mux(0, 0)
    -- regs( 2) - b_mux(3, 1), a_mux(3, 1), ..., b_mux(0, 1), a_mux(0, 1)
    -- regs( 3) - b_mux(3, 2), a_mux(3, 2), ..., b_mux(0, 2), a_mux(0, 2)
    -- regs( 4) - b_mux(3, 3), a_mux(3, 3), ..., b_mux(0, 3), a_mux(0, 3)
    -- regs( 5) - b_mux(3, 4), a_mux(3, 4), ..., b_mux(0, 4), a_mux(0, 4)
    -- regs( 6) - b_mux(3, 5), a_mux(3, 5), ..., b_mux(0, 5), a_mux(0, 5)
    -- regs( 7) - b_mux(3, 6), a_mux(3, 6), ..., b_mux(0, 6), a_mux(0, 6)
    -- regs( 8) - b_mux(3, 7), a_mux(3, 7), ..., b_mux(0, 7), a_mux(0, 7)
    -- regs( 9) - (3..2) filter_switch, (1..0) out_select
    -- Individual 1:
    -- regs(10) - b_mux(3, 0), a_mux(3, 0), ..., b_mux(0, 0), a_mux(0, 0)
    -- regs(11) - b_mux(3, 1), a_mux(3, 1), ..., b_mux(0, 1), a_mux(0, 1)
    -- regs(12) - b_mux(3, 2), a_mux(3, 2), ..., b_mux(0, 2), a_mux(0, 2)
    -- regs(13) - b_mux(3, 3), a_mux(3, 3), ..., b_mux(0, 3), a_mux(0, 3)
    -- regs(14) - b_mux(3, 4), a_mux(3, 4), ..., b_mux(0, 4), a_mux(0, 4)
    -- regs(15) - b_mux(3, 5), a_mux(3, 5), ..., b_mux(0, 5), a_mux(0, 5)
    -- regs(16) - b_mux(3, 6), a_mux(3, 6), ..., b_mux(0, 6), a_mux(0, 6)
    -- regs(17) - b_mux(3, 7), a_mux(3, 7), ..., b_mux(0, 7), a_mux(0, 7)
    -- regs(18) - (3..2) filter_switch, (1..0) out_select
    -- Individual 2:
    -- regs(19) - b_mux(3, 0), a_mux(3, 0), ..., b_mux(0, 0), a_mux(0, 0)
    -- regs(20) - b_mux(3, 1), a_mux(3, 1), ..., b_mux(0, 1), a_mux(0, 1)
    -- regs(21) - b_mux(3, 2), a_mux(3, 2), ..., b_mux(0, 2), a_mux(0, 2)
    -- regs(22) - b_mux(3, 3), a_mux(3, 3), ..., b_mux(0, 3), a_mux(0, 3)
    -- regs(23) - b_mux(3, 4), a_mux(3, 4), ..., b_mux(0, 4), a_mux(0, 4)
    -- regs(24) - b_mux(3, 5), a_mux(3, 5), ..., b_mux(0, 5), a_mux(0, 5)
    -- regs(25) - b_mux(3, 6), a_mux(3, 6), ..., b_mux(0, 6), a_mux(0, 6)
    -- regs(26) - b_mux(3, 7), a_mux(3, 7), ..., b_mux(0, 7), a_mux(0, 7)
    -- regs(27) - (3..2) filter_switch, (1..0) out_select
    -- Individual 3:
    -- regs(28) - b_mux(3, 0), a_mux(3, 0), ..., b_mux(0, 0), a_mux(0, 0)
    -- regs(29) - b_mux(3, 1), a_mux(3, 1), ..., b_mux(0, 1), a_mux(0, 1)
    -- regs(30) - b_mux(3, 2), a_mux(3, 2), ..., b_mux(0, 2), a_mux(0, 2)
    -- regs(31) - b_mux(3, 3), a_mux(3, 3), ..., b_mux(0, 3), a_mux(0, 3)
    -- regs(32) - b_mux(3, 4), a_mux(3, 4), ..., b_mux(0, 4), a_mux(0, 4)
    -- regs(33) - b_mux(3, 5), a_mux(3, 5), ..., b_mux(0, 5), a_mux(0, 5)
    -- regs(34) - b_mux(3, 6), a_mux(3, 6), ..., b_mux(0, 6), a_mux(0, 6)
    -- regs(35) - b_mux(3, 7), a_mux(3, 7), ..., b_mux(0, 7), a_mux(0, 7)
    -- regs(36) - (3..2) filter_switch, (1..0) out_select
    -- Individual 4:
    -- regs(37) - b_mux(3, 0), a_mux(3, 0), ..., b_mux(0, 0), a_mux(0, 0)
    -- regs(38) - b_mux(3, 1), a_mux(3, 1), ..., b_mux(0, 1), a_mux(0, 1)
    -- regs(39) - b_mux(3, 2), a_mux(3, 2), ..., b_mux(0, 2), a_mux(0, 2)
    -- regs(40) - b_mux(3, 3), a_mux(3, 3), ..., b_mux(0, 3), a_mux(0, 3)
    -- regs(41) - b_mux(3, 4), a_mux(3, 4), ..., b_mux(0, 4), a_mux(0, 4)
    -- regs(42) - b_mux(3, 5), a_mux(3, 5), ..., b_mux(0, 5), a_mux(0, 5)
    -- regs(43) - b_mux(3, 6), a_mux(3, 6), ..., b_mux(0, 6), a_mux(0, 6)
    -- regs(44) - b_mux(3, 7), a_mux(3, 7), ..., b_mux(0, 7), a_mux(0, 7)
    -- regs(45) - (3..2) filter_switch, (1..0) out_select
    -- Individual 5:
    -- regs(46) - b_mux(3, 0), a_mux(3, 0), ..., b_mux(0, 0), a_mux(0, 0)
    -- regs(47) - b_mux(3, 1), a_mux(3, 1), ..., b_mux(0, 1), a_mux(0, 1)
    -- regs(48) - b_mux(3, 2), a_mux(3, 2), ..., b_mux(0, 2), a_mux(0, 2)
    -- regs(49) - b_mux(3, 3), a_mux(3, 3), ..., b_mux(0, 3), a_mux(0, 3)
    -- regs(50) - b_mux(3, 4), a_mux(3, 4), ..., b_mux(0, 4), a_mux(0, 4)
    -- regs(51) - b_mux(3, 5), a_mux(3, 5), ..., b_mux(0, 5), a_mux(0, 5)
    -- regs(52) - b_mux(3, 6), a_mux(3, 6), ..., b_mux(0, 6), a_mux(0, 6)
    -- regs(53) - b_mux(3, 7), a_mux(3, 7), ..., b_mux(0, 7), a_mux(0, 7)
    -- regs(54) - (3..2) filter_switch, (1..0) out_select
    -- regs(55) - fitness of individual 0
    -- regs(56) - fitness of individual 1
    -- regs(57) - fitness of individual 2
    -- regs(58) - fitness of individual 3
    -- regs(59) - fitness of individual 4
    -- regs(60) - fitness of individual 5

    start_flag <= regs(0)(0);
    regs(0)(1) <= end_flag;
    regs(0)(2) <= fifo_rderr;
    regs(0)(3) <= fifo_wrerr;
    regs(0)(reg_t'high downto 4) <= frame_count;

    chrom_intconn: for i in 0 to population-1 generate
        chrom_col: for j in 0 to columns-1 generate
            chrom_row: for k in 0 to rows-1 generate
                chromosome(i).a_mux(k, j) <= regs(i*(columns+1)+j+1)(
                    input_sel_width*(2*k+1)-1 downto input_sel_width*2*k);
                chromosome(i).b_mux(k, j) <= regs(i*(columns+1)+j+1)(
                    input_sel_width*2*(k+1)-1 downto input_sel_width*(2*k+1));
            end generate;
        end generate;

        chromosome(i).out_select <= regs((i+1)*(columns+1))(
            output_sel_width-1 downto 0*output_sel_width);
        chromosome(i).filter_switch <= regs((i+1)*(columns+1))(
            2*output_sel_width-1 downto 1*output_sel_width);
    end generate;

    fitness_intconn: for i in numregisters-population to numregisters-1
    generate
        regs(i)(fitness_t'range) <= fitness(i-(numregisters-population));
        regs(i)(reg_t'high downto fitness_t'high+1) <= (others => '0');
    end generate;

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
                writeaddr <= (others => '0');
                wready <= '0';
                bvalid  <= '0';
                arready <= '0';
                readaddr  <= (others => '1');
                rvalid <= '0';
                rdata  <= (others => '0');
            else
                if (awready = '0' and s_axi_awvalid = '1' and
                        s_axi_wvalid = '1') then
                    awready <= '1';
                else
                    awready <= '0';
                end if;

                if (awready = '0' and s_axi_awvalid = '1' and
                        s_axi_wvalid = '1') then
                    writeaddr <= s_axi_awaddr(reg_bits+word_bits-1 downto
                                 word_bits);
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
                    readaddr  <= s_axi_araddr(reg_bits+word_bits-1 downto
                                 word_bits);
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

    registers: for i in regs'range generate

        -- in the control register only the start bit is writable (and is
        -- resetable by the signal from the cgp core)
        control_wr: if i = 0 generate
            process (s_axi_aclk, start_rst) is
            begin
                if start_rst = '1' then
                    regs(0)(0) <= '0';
                else
                    if rising_edge(s_axi_aclk) then
                        if (reg_wren = '1' and
                            writeaddr =
                            std_logic_vector(to_unsigned(0, reg_bits)))
                        then
                            if s_axi_wstrb(0) = '1' then
                                regs(0)(0) <= s_axi_wdata(0);
                            end if;
                        end if;
                    end if;
                end if;
            end process;
        end generate;

        -- the chromosome registers are resetable and writable regular
        -- registers
        chromosome_wr: if ((i > 0) and (i < (numregisters-population)))
        generate
            write_process: process (s_axi_aclk) is
            begin
                if rising_edge(s_axi_aclk) then
                    if s_axi_aresetn = '0' then
                        regs(i) <= (others => '0');
                    else
                        if (reg_wren = '1' and
                                writeaddr =
                                std_logic_vector(to_unsigned(i, reg_bits)))
                        then
                            for j in 0 to w32bytes-1 loop
                                if s_axi_wstrb(j) = '1' then
                                    regs(i)((j+1)*8-1 downto j*8) <=
                                        s_axi_wdata((j+1)*8-1 downto j*8);
                                end if;
                            end loop;
                        end if;
                    end if;
                end if;
            end process;
        end generate;

        -- the fitness registers are read-only
    end generate;

    databusgen: for i in regs'range generate
        process (regs, readaddr, s_axi_aresetn)
        begin
            if s_axi_aresetn = '0' then
                databus <= (others => '1');
            else
                if readaddr = std_logic_vector(to_unsigned(i, reg_bits)) then
                    databus <= regs(i);
                else
                    databus <= (others => 'Z');
                end if;
            end if;
        end process;
    end generate;

end beh_axi_evol;
