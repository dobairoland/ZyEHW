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

PROJECT_NAME1	:= img_manip
PROJECT_NAME2	:= out_proc

#################################

CC		:= gcc
RM		:= rm -f *.o

XMLFLAGS	= `xml2-config --cflags`
XMLLIBS		= `xml2-config --libs`
IMGLIBS		= -ltiff

#################################

build: $(PROJECT_NAME1) $(PROJECT_NAME2)

#################################

$(PROJECT_NAME1): img.o tif_handler.o $(PROJECT_NAME1).o
	$(CC) $(FLAGS) -o $@ $^ $(IMGLIBS)

$(PROJECT_NAME2): xml.o img.o tif_handler.o cgp.o $(PROJECT_NAME2).o
	$(CC) $(FLAGS) -o $@ $^ $(XMLLIBS) $(IMGLIBS)

#################################

%.o: %.c
	$(CC) $(XMLFLAGS) -c -o $@ $<

#################################

rebuild: clean build

#################################


.PHONY: clean
clean:
	$(RM)

#################################

help:
	@echo "Makefile usage:"
	@echo "         make [rebuild]"
	@echo "         make clean"
	@echo "         make help"

#################################
