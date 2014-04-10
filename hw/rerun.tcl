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

open_project prj/prj.xpr

reset_run synth_1
reset_run impl_1

launch_runs synth_1
wait_on_run synth_1

launch_runs impl_1 -to_step write_bitstream
wait_on_run impl_1

open_run impl_1
