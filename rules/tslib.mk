tslib-configure:
	cd tslib && ./autogen.sh && ./configure --host=arm-linux --prefix=$(INSTALLDIR)/tslib ac_cv_func_malloc_0_nonnull=yes --cache-file=arm-linux.cache CC=$(CC)
tslib:
	make -C tslib

tslib-clean:
	make -C tslib clean

tslib-install:
	make -C tslib install
