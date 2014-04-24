
all: libs

LIBS = libfem.a

CROSS_COMPILE = arm-arago-linux-gnueabi-
CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld

# Dependencies
DEPEND_FILES	= common.o liblcd.o syspicture.o fem_fb.o asc1224.o

DEBUG	= -g -Wall
IFLAGS 	+= -I./
LDFLAGS	+= -pthread

libs: $(DEPEND_FILES)
	$(AR) $(ARFLAGS) $(LIBS) $?

clean:
	rm -f $(LIBS) $(DEPEND_FILES)

common.o: common.c common.h

.c.o:
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
