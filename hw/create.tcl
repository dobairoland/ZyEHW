# Copyright (C) 2014 Roland Dobai
#
# This file is part of ZyEHW.
#
# ZyEHW is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# ZyEHW is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along
# with ZyEHW. If not, see <http://www.gnu.org/licenses/>.

set design_name zyehw
set sources [ glob ./hdl/*.vhd ]
set testbenches [ glob ./hdl/test/*.vhd ]
set constraints ./hdl/$design_name.xdc
set bd_name dsgn
set prj_name prj
set bd_path ${prj_name}/${prj_name}.srcs/sources_1/bd/${bd_name}
set bd_file ${bd_path}/${bd_name}.bd
set bd_wr_path ${bd_path}/hdl/${bd_name}_wrapper.vhd
set ver [version -short]

create_project ${prj_name} ${prj_name} -force -part xc7z020clg484-1
set_property target_language VHDL [current_project]

if { [string match "2014.?" $ver] } {
    set_property board em.avnet.com:zed:part0:0.9 [current_project]
} else {
    # the name was different before version 2014.1
    set_property board em.avnet.com:zynq:zed:c [current_project]
}

create_bd_design ${bd_name}

if { [string match "2014.?" $ver] } {
    create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.4 \
        processing_system7_0
} elseif { [string match "2013.4" $ver] } {
    create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.3 \
        processing_system7_0
} else {
    puts "Version $ver is not supported";
    exit;
}

# Design automation removes some settings (especially in 2013.4) therefore this
# needs to be done in the beginning
apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 \
    -config {make_external "FIXED_IO, DDR" apply_board_preset "1" } \
    [get_bd_cells processing_system7_0]

# master AXI ports on
# enabling interrupts
# enabling PL -> PS global interrupts
set_property -dict [list \
    CONFIG.PCW_USE_M_AXI_GP0 {1} \
    CONFIG.PCW_USE_M_AXI_GP1 {1} \
    CONFIG.PCW_USE_FABRIC_INTERRUPT {1} \
    CONFIG.PCW_IRQ_F2P_INTR {1} \
    CONFIG.PCW_EN_CLK0_PORT {1} \
    CONFIG.PCW_EN_RST0_PORT {1} \
    ] [get_bd_cells processing_system7_0]

create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 \
    axi_interconnect_0
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 \
    axi_interconnect_1

# 1 master and 1 slave
set_property -dict [list \
    CONFIG.NUM_MI {1} \
    CONFIG.NUM_SI {1} \
    ] [get_bd_cells axi_interconnect_0]

# 1 master and 1 slave
set_property -dict [list \
    CONFIG.NUM_MI {1} \
    CONFIG.NUM_SI {1} \
    ] [get_bd_cells axi_interconnect_1]

# connections PS <-> AXI interconnect 0
connect_bd_net \
    [get_bd_pins processing_system7_0/FCLK_CLK0] \
    [get_bd_pins processing_system7_0/M_AXI_GP0_ACLK]
connect_bd_net -net [get_bd_nets processing_system7_0_fclk_clk0] \
    [get_bd_pins axi_interconnect_0/ACLK] \
    [get_bd_pins processing_system7_0/FCLK_CLK0]
connect_bd_net -net [get_bd_nets processing_system7_0_fclk_clk0] \
    [get_bd_pins axi_interconnect_0/S00_ACLK] \
    [get_bd_pins processing_system7_0/FCLK_CLK0]
connect_bd_net -net [get_bd_nets processing_system7_0_fclk_clk0] \
    [get_bd_pins axi_interconnect_0/M00_ACLK] \
    [get_bd_pins processing_system7_0/FCLK_CLK0]
connect_bd_net \
    [get_bd_pins processing_system7_0/FCLK_RESET0_N] \
    [get_bd_pins axi_interconnect_0/ARESETN]
connect_bd_net -net [get_bd_nets processing_system7_0_fclk_reset0_n] \
    [get_bd_pins axi_interconnect_0/S00_ARESETN] \
    [get_bd_pins processing_system7_0/FCLK_RESET0_N]
connect_bd_net -net [get_bd_nets processing_system7_0_fclk_reset0_n] \
    [get_bd_pins axi_interconnect_0/M00_ARESETN] \
    [get_bd_pins processing_system7_0/FCLK_RESET0_N]
connect_bd_intf_net [get_bd_intf_pins axi_interconnect_0/S00_AXI] \
    [get_bd_intf_pins processing_system7_0/M_AXI_GP0]

# connections PS <-> AXI interconnect 1
connect_bd_net \
    [get_bd_pins processing_system7_0/FCLK_CLK0] \
    [get_bd_pins processing_system7_0/M_AXI_GP1_ACLK]
connect_bd_net -net [get_bd_nets processing_system7_0_fclk_clk0] \
    [get_bd_pins axi_interconnect_1/ACLK] \
    [get_bd_pins processing_system7_0/FCLK_CLK0]
connect_bd_net -net [get_bd_nets processing_system7_0_fclk_clk0] \
    [get_bd_pins axi_interconnect_1/S00_ACLK] \
    [get_bd_pins processing_system7_0/FCLK_CLK0]
connect_bd_net -net [get_bd_nets processing_system7_0_fclk_clk0] \
    [get_bd_pins axi_interconnect_1/M00_ACLK] \
    [get_bd_pins processing_system7_0/FCLK_CLK0]
connect_bd_net \
    [get_bd_pins processing_system7_0/FCLK_RESET0_N] \
    [get_bd_pins axi_interconnect_1/ARESETN]
connect_bd_net -net [get_bd_nets processing_system7_0_fclk_reset0_n] \
    [get_bd_pins axi_interconnect_1/S00_ARESETN] \
    [get_bd_pins processing_system7_0/FCLK_RESET0_N]
connect_bd_net -net [get_bd_nets processing_system7_0_fclk_reset0_n] \
    [get_bd_pins axi_interconnect_1/M00_ARESETN] \
    [get_bd_pins processing_system7_0/FCLK_RESET0_N]
connect_bd_intf_net [get_bd_intf_pins axi_interconnect_1/S00_AXI] \
    [get_bd_intf_pins processing_system7_0/M_AXI_GP1]

# making AXI data ports external
create_bd_intf_port -mode Master \
    -vlnv xilinx.com:interface:aximm_rtl:1.0 M00_AXI_0
connect_bd_intf_net [get_bd_intf_pins axi_interconnect_0/M00_AXI] \
    [get_bd_intf_ports /M00_AXI_0]
create_bd_intf_port -mode Master \
    -vlnv xilinx.com:interface:aximm_rtl:1.0 M00_AXI_1
connect_bd_intf_net [get_bd_intf_pins axi_interconnect_1/M00_AXI] \
    [get_bd_intf_ports /M00_AXI_1]

# making AXI clock external
create_bd_port -dir O -type clk ACLK
connect_bd_net -net [get_bd_nets processing_system7_0_fclk_clk0] \
    [get_bd_ports ACLK] [get_bd_pins processing_system7_0/FCLK_CLK0]

# associating AXI data and clock output
set_property CONFIG.ASSOCIATED_BUSIF {M00_AXI_1:M00_AXI_0} [get_bd_ports /ACLK]

# setting protocol for AXI data outputs
set_property CONFIG.PROTOCOL AXI4LITE [get_bd_intf_ports M00_AXI_0]
set_property CONFIG.PROTOCOL AXI4LITE [get_bd_intf_ports M00_AXI_1]

create_bd_addr_seg -range 4K -offset 0x43C00000 \
    [get_bd_addr_spaces processing_system7_0/Data] \
    [get_bd_addr_segs M00_AXI_0/Reg] seg0
create_bd_addr_seg -range 4K -offset 0x83C00000 \
    [get_bd_addr_spaces processing_system7_0/Data] \
    [get_bd_addr_segs M00_AXI_1/Reg] seg1

create_bd_port -dir I -from 0 -to 0 -type intr irq_f2p
connect_bd_net [get_bd_pins /processing_system7_0/IRQ_F2P] \
    [get_bd_ports /irq_f2p]

# setting the clock frequency (valid range 0.1 - 250)
# setting this with the other settings at the beginning did not work
set_property -dict [list \
    CONFIG.PCW_FPGA0_PERIPHERAL_FREQMHZ {100} \
    ] [get_bd_cells processing_system7_0]

save_bd_design
generate_target all [get_files ${bd_file}]

make_wrapper -files [get_files ${bd_file}] -top
import_files -force -norecurse ${bd_wr_path}

add_files ${sources}
add_files -fileset constrs_1 ${constraints}
add_files -fileset sim_1 ${testbenches}

update_compile_order -fileset sources_1
set_property top ${design_name} [current_fileset]
update_compile_order -fileset sources_1
update_compile_order -fileset sim_1

export_hardware [get_files ${bd_file}]
close_bd_design [get_bd_designs ${bd_name}]

# In order to meet the timing constraints Performance_Explore strategy is
# used.  Another good option would be Performance_RefinePlacement but the
# runtime would be higher.
set_property strategy Performance_Explore [get_runs impl_1]

set project_path [get_property directory [current_project]]

# pre_opt_design.tcl will be executed before opt_design
set_property STEPS.OPT_DESIGN.TCL.PRE $project_path/../pre_opt_design.tcl \
    [get_runs impl_1]
