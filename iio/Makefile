
all:	compile 

LIBS	= libiio.a
NAME	= iio

CROSS_COMPILE = arm-arago-linux-gnueabi-
CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld

# Dependencies
DEPEND_FILES	= iio.o

DEBUG	= -g -Wall
IFLAGS 	+= -I./
LDFLAGS	+= -pthread

compile: $(NAME)

#
#	Build LIBSive of objects
#
$(LIBS): $(DEPEND_FILES)
	$(AR) $(ARFLAGS) $(LIBS) $?

#
#	Primary link
#
$(NAME): Makefile $(LIBS)
	$(CC) -o $(NAME) $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		main.c $(LIBS) $(LDFLAGS) 

	@cp $(NAME) /home/xzwang/nfs/mnt
clean:
	rm -f $(NAME) $(LIBS) $(DEPEND_FILES)

#
#	Dependencies
#
iio.o: iio.c iio.h

#
#	Transition rules (add -o to put object in right directory)
#
.c.o:
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
