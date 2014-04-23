fftw-configure:
	cd fftw && ./configure --host=arm-linux --prefix=$(INSTALLDIR)/fftw CC=$(CC)

fftw: fftw-configure
	make -j 4 -C fftw

fftw-clean:
	make -C fftw clean

fftw-install:
	mkdir -p $(INSTALLDIR)/fftw
	make -C fftw install
