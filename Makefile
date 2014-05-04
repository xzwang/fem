#########################################################################
#			Makefile for FEM Instrument			#
#									#
#########################################################################


#####################ENVIRMENT PATH#####################
export TOP :=$(shell pwd)
export SRCBASE:=$(TOP)
export ARCH=arm
export PLATFORM=AM335X

V ?= V1.0.0

#####################CROSS_COMPILE######################
export CROSS_COMPILE =arm-arago-linux-gnueabi-
export CC := $(CROSS_COMPILE)gcc
export CXX := $(CROSS_COMPILE)g++
export AR := $(CROSS_COMPILE)ar
export AS := $(CROSS_COMPILE)as
export LD := $(CROSS_COMPILE)ld
export NM := $(CROSS_COMPILE)nm
export RANLIB := $(CROSS_COMPILE)ranlib
export STRIP := $(CROSS_COMPILE)strip
export SIZE := $(CROSS_COMPILE)size
export CFLAGS := -I$(TOP)/common	#-Wpointer-arith
export LDFLAGS := -L$(TOP)/common -lfem


#####################IMAGE DIR#########################
export PLATFORMDIR := $(TOP)/$(PLATFORM)
export INSTALLDIR := $(PLATFORMDIR)/install
export TARGETDIR := $(PLATFORMDIR)/rootfs

include .config
include rules/config.mk
obj-configure := $(foreach obj, $(obj-y), $(obj)-configure)
obj-clean := $(foreach obj,$(obj-y) $(obj-n),$(obj)-clean)
obj-install := $(foreach obj,$(obj-y),$(obj)-install)
obj-distclean := $(foreach obj,$(obj-y) $(obj-n),$(obj)-distclean)

all: build $(obj-y)

build:
	echo "#define BUILD_DATE \"$(shell date +%D)\"">./common/build.h
	echo "#define WARE_VERSION \"$(V)\"" >>./common/build.h
	make -C common

clean: $(obj-clean)
	make -C common clean

clean_target:
	rm -rf $(INSTALLDIR)
	rm -rf $(TARGETDIR)

distclean:$(obj-distclean) clean_target

install: clean_target $(obj-install)
	mkdir -p $(TARGETDIR)
	for dir in $(wildcard $(patsubst %,$(INSTALLDIR)/%,$(obj-y))); do \
	(cd $${dir} && tar cpf - .) | (cd $(TARGETDIR) && tar xpf -)\
	done
	mkdir -p $(TARGETDIR)/dev
	sudo mknod $(TARGETDIR)/dev/console c 5 1
	sudo mknod $(TARGETDIR)/dev/null c 1 3
	sudo mknod $(TARGETDIR)/dev/nvram c 229 0
	mkdir -p $(TARGETDIR)/proc
	mkdir -p $(TARGETDIR)/sys
	mkdir -p $(TARGETDIR)/tmp
	mkdir -p $(TARGETDIR)/mnt
	mkdir -p $(TARGETDIR)/home
	cp -aRf $(PLATFORM)/lib $(TARGETDIR)/
	cp -aRf $(PLATFORM)/etc $(TARGETDIR)/etc

include rules/all.mk
.PHONY:	all build clean $(obj-y) $(obj-clean) $(obj-install) $(obj-distclean) $(obj-configure)
.PHONY:	dummy

