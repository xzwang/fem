
fft:
	make -C fft
fft-clean:
	make -C fft clean

fft-install:
	mkdir -p $(INSTALLDIR)/fft/usr/sbin
	cp fft/fft $(INSTALLDIR)/fft/usr/sbin


