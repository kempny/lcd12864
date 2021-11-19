#
# Makefile:
#	This file is part of stepmotor - RaspberryPi stepper motor 
#       control library using A4988 controller
#       Copyright (C) 2021  Boguslaw Kempny kempny@stanpol.com.pl
#
#################################################################################
#    stepmotor is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    stepmotor is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
#
#################################################################################



DEBUG	= -O2
CC	= gcc
INCLUDE	= -I.
DEFS	= -D_GNU_SOURCE
CFLAGS	= $(DEFS) $(LIBS) 

LIBS    =  -lpthread -llcd12864 -lwiringPi

###############################################################################

#SRC	=	demo.c demo1.c test.c
SRC	=	test.c

#OBJ	=	$(SRC:.c=.o)
#BINS    =       $(SRC:.c=)


#EXECS = $(SRC:%.c=%)

all: $(SRC:%.c=%)


# DO NOT DELETE
