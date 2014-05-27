iio:
	make -C iio

iio-install:
	mkdir -p $(INSTALLDIR)/iio/usr/sbin
	mkdir -p $(INSTALLDIR)/iio/usr/lib
	cp iio/iio $(INSTALLDIR)/iio/usr/sbin
	cp iio/libiio.a $(INSTALLDIR)/iio/usr/lib

iio-clean:
	make -C iio clean
