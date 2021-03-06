include rules/iio.mk
include rules/fftw.mk
include rules/fft.mk
include rules/busybox.mk
include rules/udev.mk
include rules/tslib.mk
include rules/sdl.mk

%:
	[ ! -d $* ] || $(MAKE) -C $*  

%-distclean:
	[ ! -d $* ] || $(MAKE) -C $* clean

%-clean:
	[ ! -d $* ] || $(MAKE) -C $* clean

%-install:
	[ ! -d $* ] || $(MAKE) -C $* install INSTALLDIR=$(INSTALLDIR)/$*

$(obj-y) $(obj-m) $(obj-n) $(obj-clean) $(obj-install): dummy

