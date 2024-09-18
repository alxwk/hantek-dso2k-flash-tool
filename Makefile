# SPDX-License-Identifier: MIT
# Copyright 2022-2024 DavidAlfa
# Copyright 2007-2022 Jianjun Jiang <8192542@qq.com>

#
# Top makefile
#

CROSS		?=

AS			:= $(CROSS)gcc -x assembler-with-cpp
CC			:= $(CROSS)gcc
CXX			:= $(CROSS)g++
LD			:= $(CROSS)ld
AR			:= $(CROSS)ar
OC			:= $(CROSS)objcopy
OD			:= $(CROSS)objdump
RM			:= rm -fr

ASFLAGS		:= -g -ggdb -Wall -O2
CFLAGS		:= -g -ggdb -Wall -O2
CXXFLAGS	:= -g -ggdb -Wall -O2
LDFLAGS		:=
ARFLAGS		:= -rcs
OCFLAGS		:= -v -O binary
ODFLAGS		:=
MCFLAGS		:=

LIBDIRS		:=
LIBS 		:= -L". /usr/local/lib /usr/i686-w64-mingw32/lib" -lusb-1.0

INCDIRS		:= -I". /usr/include/libusb-1.0 /usr/i686-w64-mingw32/include/libusb-1.0"
SRCDIRS		:= . chips


SFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
CPPFILES	:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))

SDEPS		:= $(patsubst %, %, $(SFILES:.S=.o.d))
CDEPS		:= $(patsubst %, %, $(CFILES:.c=.o.d))
CPPDEPS		:= $(patsubst %, %, $(CPPFILES:.cpp=.o.d))
DEPS		:= $(SDEPS) $(CDEPS) $(CPPDEPS)

SOBJS		:= $(patsubst %, %, $(SFILES:.S=.o))
COBJS		:= $(patsubst %, %, $(CFILES:.c=.o))
CPPOBJS		:= $(patsubst %, %, $(CPPFILES:.cpp=.o))
OBJS		:= $(SOBJS) $(COBJS) $(CPPOBJS)

OBJDIRS		:= $(patsubst %, %, $(SRCDIRS))
NAME		:= dsoflash
VPATH		:= $(OBJDIRS)

.PHONY:		all install clean

all : $(NAME)

$(NAME) : $(OBJS)
	@echo [LD] Linking $@
	@$(CC) $(LDFLAGS) $(LIBDIRS) $^ -o $@ $(LIBS)

$(SOBJS) : %.o : %.S
	@echo [AS] $<
	@$(AS) $(ASFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

$(COBJS) : %.o : %.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

$(CPPOBJS) : %.o : %.cpp
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

install:
	install -Dm0755 dsoflash /usr/local/bin/dsoflash
	install -Dm0644 99-dsoflash.rules /etc/udev/rules.d/99-dsoflash.rules
	install -Dm0644 LICENSE /usr/share/licenses/dsoflash/LICENSE
	udevadm control --reload

clean:
	@$(RM) $(DEPS) $(OBJS) $(NAME).exe $(NAME) *~
