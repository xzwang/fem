iio:
	make -C iio

iio-install:
	mkdir -p $(INSTALLDIR)/iio/sbin
	mkdir -p $(INSTALLDIR)/iio/lib
	cp iio/iio $(INSTALLDIR)/iio/sbin	
	cp iio/libiio.a $(INSTALLDIR)/iio/lib

iio-clean:
	make -C iio clean
