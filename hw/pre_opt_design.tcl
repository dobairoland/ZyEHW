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
#

# The Block RAMs from the FIFO should be excluded from the chip-enable
# optimization performed during opt_design. This would insert some logic and
# the timing would not be met.
set_power_opt -exclude_cells \
    [get_cells -filter {PRIMITIVE_TYPE =~ BMEM.*.*} \
        ehw_core_i/fifo_sync_i/fifo_i/ram_i/*]
