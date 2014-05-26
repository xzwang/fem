sdl-configure:
	cd sdl && ./configure --prefix=$(INSTALLDIR)/sdl   --host=arm-linux --target=arm-linux --disable-debug --disable-cdrom --enable-threads --enable-timers --enable-endian --enable-file --enable-oss --disable-alsa --disable-esd --disable-arts --disable-diskaudio --disable-nas --disable-esd-shared --disable-esdtest --disable-mintaudio --disable-nasm --disable-video-x11 --disable-video-dga --enable-video-fbcon --enable-video-directfb --disable-video-ps2gs --disable-video-xbios --disable-video-gem --enable-video-opengl --enable-pthreads --enable-video-qtopia --enable-dlopen --disable-directx --disable-stdio-redirect CC=$(CC) --enable-input-events --enable-input-tslib CFLAGS="-I$(INSTALLDIR)/tslib/include" LDFLAGS="-L$(INSTALLDIR)/tslib/lib -lts"
sdl:
	make -C sdl

sdl-clean:
	make -C sdl clean

sdl-distclean:
	make -C sdl distclean

sdl-install:
	make -C sdl install
