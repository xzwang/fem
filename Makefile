#########################################################################
#			Makefile for FEM Instrument			#
#									#
#########################################################################


#####################ENVIRMENT PATH#####################
export TOP :=$(shell pwd)
export SRCBASE:=/opt/fem
export ARCH=arm
export PLATFORM=AM335X
export LINUXDIR :=/opt/kernel

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


#####################IMAGE DIR#########################
export PLATFORMDIR := $(TOP)/$(PLATFORM)
export INSTALLDIR := $(PLATFORMDIR)/install
export TARGETDIR := $(PLATFORMDIR)/target

include .config
include rules/config.mk
obj-clean := $(foreach obj,$(obj-y) $(obj-n),$(obj)-clean)
obj-install := $(foreach obj,$(obj-y),$(obj)-install)
obj-distclean := $(foreach obj,$(obj-y) $(obj-n),$(obj)-distclean)


all: build $(obj-y)

build:
	echo "#define BUILD_DATE \"$(shell date +%D)\"">./build.h
	echo "#define WARE_VERSION \"$(V)\"" >>./build.h

clean: $(obj-clean)
	echo "clean"

clean_target:
	rm -rf $(INSTALLDIR)
	rm -rf $(TARGETDIR)

distclean:$(obj-distclean) clean_target

install: clean_target $(obj-install)
	mkdir -p $(TARGETDIR)
	for dir in $(wildcard $(patsubst %,$(INSTALLDIR)/%,$(obj-y))); do \
	(cd $${dir} && tar cpf - .) | (cd $(TARGETDIR) && tar xpf -)\
	done


include rules/all.mk
.PHONY:	all build clean
.PHONY:	dummy

