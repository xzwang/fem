
fft:
	make -C fft
fft-clean:
	make -C fft clean

fft-install:
	mkdir -p $(INSTALLDIR)/fft/sbin
	cp fft/fft $(INSTALLDIR)/fft/sbin


