include rules/iio.mk
include rules/fftw.mk
include rules/fft.mk
include rules/busybox.mk

%:
	[ ! -d $* ] || $(MAKE) -C $*  

%-distclean:
	[ ! -d $* ] || $(MAKE) -C $* clean

%-clean:
	[ ! -d $* ] || $(MAKE) -C $* clean

%-install:
	[ ! -d $* ] || $(MAKE) -C $* install INSTALLDIR=$(INSTALLDIR)/$*

$(obj-y) $(obj-m) $(obj-n) $(obj-clean) $(obj-install): dummy

