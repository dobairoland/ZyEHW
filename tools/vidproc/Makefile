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

PROJECT_NAME1	:= vidprocsim
PROJECT_NAME2	:= testvid
PROJECT_NAME3	:= check

#################################

CC		= gcc
RM		= rm -f *.o

FLAGS		= -march=native -O2 -Wall -Wextra -pedantic
XMLFLAGS	= $(FLAGS) `xml2-config --cflags`
LIBS		= -lm
XMLLIBS		= `xml2-config --libs`
IMGLIBS		= -ltiff
#################################

build: $(PROJECT_NAME1) $(PROJECT_NAME2) $(PROJECT_NAME3)

#################################

$(PROJECT_NAME1): operators.o cgp.o xml.o frm.o video.o noise.o tif_handler.o \
		$(PROJECT_NAME1).o
	$(CC) $(FLAGS) -o $@ $^ $(XMLLIBS) $(IMGLIBS)

$(PROJECT_NAME2): frm.o video.o noise.o $(PROJECT_NAME2).o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

$(PROJECT_NAME3): operators.o frm.o cgp.o xml.o $(PROJECT_NAME3).o
	$(CC) $(FLAGS) -o $@ $^ $(XMLLIBS)

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
