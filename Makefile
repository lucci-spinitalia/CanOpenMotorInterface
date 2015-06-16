#! gmake

#
# Copyright (C) 2006 Laurent Bessard
# 
# This file is part of canfestival, a library implementing the canopen
# stack
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# 

CC = gcc
CXX = g++
LD = g++
OPT_CFLAGS = -O2
CFLAGS = $(OPT_CFLAGS)
PROG_CFLAGS = 
EXE_CFLAGS =  -lpthread -lrt -ldl
OS_NAME = Linux
ARCH_NAME = arm
PREFIX = /usr/local
TARGET = unix
CAN_DRIVER = can_socket
TIMERS_DRIVER = timers_unix
CANOPENSHELL = CANOpenShell
CANFESTIVAL_DIR = /home/debian/CanFestival-3-7740ac6fdedc

INCLUDES = -I$(CANFESTIVAL_DIR)/include -I$(CANFESTIVAL_DIR)/include/$(TARGET) -I$(CANFESTIVAL_DIR)/include/$(CAN_DRIVER) -I$(CANFESTIVAL_DIR)/include/$(TIMERS_DRIVER)

MASTER_OBJS = CANOpenShellMasterOD.o CANOpenShell.o CANOpenShellMasterError.o CANOpenShellStateMachine.o

OBJS = $(MASTER_OBJS) $(CANFESTIVAL_DIR)/src/libcanfestival.a $(CANFESTIVAL_DIR)/drivers/$(TARGET)/libcanfestival_$(TARGET).a

ifeq ($(TARGET),win32)
	CANOPENSHELL = CANOpenShell.exe
endif

ifeq ($(TIMERS_DRIVER),timers_win32)
	EXE_CFLAGS =
endif

ifeq ($(TIMERS_DRIVER),timers_xeno)
	PROGDEFINES = -DUSE_XENO
endif

ifeq ($(TIMERS_DRIVER),timers_rtai)
	PROGDEFINES = -DUSE_RTAI
endif

all: $(CANOPENSHELL)

$(CANFESTIVAL_DIR)/drivers/$(TARGET)/libcanfestival_$(TARGET).a:
	$(MAKE) -C $(CANFESTIVAL_DIR)/drivers/$(TARGET) libcanfestival_$(TARGET).a

$(CANOPENSHELL): $(OBJS)
	$(LD) $(CFLAGS) $(PROG_CFLAGS) ${PROGDEFINES} $(INCLUDES) -o $@ $(OBJS) $(EXE_CFLAGS)
	mkdir -p Debug; cp $(CANOPENSHELL) Debug
	
CANOpenShellMasterOD.c: CANOpenShellMasterOD.od
	$(MAKE) -C $(CANFESTIVAL_DIR)/objdictgen gnosis
	python $(CANFESTIVAL_DIR)/objdictgen/objdictgen.py CANOpenShellMasterOD.od CANOpenShellMasterOD.c

%.o: %.c
	$(CC) $(CFLAGS) $(PROG_CFLAGS) ${PROGDEFINES} $(INCLUDES) -o $@ -c $<

clean:
	rm -f $(MASTER_OBJS)
	rm -f $(CANOPENSHELL)
		
mrproper: clean
	rm -f CANOpenShellMasterOD.c

install: $(CANOPENSHELL)
	mkdir -p $(DESTDIR)$(PREFIX)/bin/
	cp $< $(DESTDIR)$(PREFIX)/bin/
	
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(CANOPENSHELL)
