
all:	compile 

ARCH	= libiio.a
NAME	= iio

CROSS_COMPILE = arm-arago-linux-gnueabi-
CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld

# Dependencies
DEPEND_FILES	= iio.o

DEBUG	= -g -Wall
IFLAGS 	= -I./
LDFLAGS	= -pthread

compile: $(NAME)

#
#	Build archive of objects
#
$(ARCH): $(DEPEND_FILES)
	$(AR) $(ARFLAGS) $(ARCH) $?

#
#	Primary link
#
$(NAME): Makefile $(ARCH)
	$(CC) -o $(NAME) $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		main.c $(ARCH) $(LDFLAGS) 

	@cp $(NAME) /home/xzwang/nfs/mnt
clean:
	rm -f $(NAME) $(ARCH) $(DEPEND_FILES)

#
#	Dependencies
#
iio.o: iio.c iio.h

#
#	Transition rules (add -o to put object in right directory)
#
.c.o:
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
