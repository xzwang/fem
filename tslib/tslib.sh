#!/bin/sh
#./ autogen-clean.sh
./autogen.sh

./configure --host=arm-linux ac_cv_func_malloc_0_nonnull=yes --cache-file=arm-linux.cache -prefix=/usr/local/arm/tslib

make

make install
