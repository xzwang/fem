udev-configure:
	cd udev && ./configure --host=arm-linux --prefix=$(INSTALLDIR)/udev CROSS_COMPILE=$(CROSS_COMPILE)
udev:
	make -C udev CROSS_COMPILE=$(CROSS_COMPILE)

udev-clean:
	make -C udev clean

udev-install:
	make -C udev install DESTDIR=$(INSTALLDIR)/udev

