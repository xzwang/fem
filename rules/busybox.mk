busybox:
	cp busybox/.config_std busybox/.config
	$(MAKE) -j 4 -C busybox   CONFIG_PREFIX=$(INSTALLDIR)/busybox ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

busybox-install:
	$(MAKE) -j 4 -C busybox STRIPTOOL=$(STRIP) CONFIG_PREFIX=$(INSTALLDIR)/busybox install

busybox-clean:
	$(MAKE) -j 4  -C busybox clean

busybox-distclean:
	$(MAKE) -C busybox distclean
