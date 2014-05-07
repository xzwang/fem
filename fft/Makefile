######################################################################
#	Makefile for FFT lib
######################################################################

ifeq "$(ARCH)" "arm"
#CROSS_COMPILE = arm-arago-linux-gnueabi-
#CC=$(CROSS_COMPILE)gcc
#AR=$(CROSS_COMPILE)ar
#LD=$(CROSS_COMPILE)ld
CFLAGS += -I$(TOP)/$(PLATFORM)/install/fftw/include -static
LDFLAGS += -L$(TOP)/$(PLATFORM)/install/fftw/lib -lfftw3f -lm

else
CFLAGS = -I./fft_for_pc/include -static
LDFLAGS = -L./fft_for_pc/lib -lfftw3f -lm
endif

.PHONY:all

DEBUG= -g -Wall
FFTW=fft
SRC=fem-fft.c
OBJ= fft_float.o# fft_double.o
LIB=libfft.a

all: $(FFTW)

$(LIB):$(OBJ)

	$(AR) r $@ $?

$(FFTW):$(SRC) $(OBJ)
	$(CC) -o $@ $(DEBUG) $(CFLAGS) $^ $(LDFLAGS)

clean:
	rm -f $(FFTW) $(LIB) *.o

#	Transition rules (add -o to put object in right directory)
#
.c.o:
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
