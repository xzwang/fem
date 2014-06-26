LDFLAGS += -pthread

DEBUG= -g -Wall
NAME=fpga
SRC=fpga-test.c
OBJ= nl_msg.o nl_pool.o
LIB = libfft.a

all: $(NAME)

$(LIB):$(OBJ)

	$(AR) r $@ $?

$(NAME):$(SRC) $(LIB)
	$(CC) -o $@ $(DEBUG) $(CFLAGS) $^ $(LDFLAGS)

clean:
	rm -f $(NAME) $(LIB) *.o

#	Transition rules (add -o to put object in right directory)
#
.c.o:
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

.PHONY:all
