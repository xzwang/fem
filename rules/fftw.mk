fftw-configure:
	cd fftw && ./configure --host=arm-linux --prefix=$(INSTALLDIR)/fftw CC=$(CC) LDFLAGS='-L.' CFLAGS='-O3' --enable-neon --enable-float
#	cd fftw && ./configure --host=arm-linux --prefix=$(INSTALLDIR)/fftw CC=$(CC) LDFLAGS='-L.' CFLAGS='-O3 -fomit-frame-pointer -fstrict-aliasing -fno-schedule-insns -ffast-math'

fftw:
	make -j 4 -C fftw

fftw-clean:
	make -C fftw clean

fftw-install:
	mkdir -p $(INSTALLDIR)/fftw
	make -C fftw install
