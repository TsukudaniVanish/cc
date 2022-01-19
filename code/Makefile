CFLAGS=-std=c11 -g -static
SRC=$(wildcard *.c)
SRCSS=$(filter-out test.c, $(SRC))
SRCS=$(filter-out test_2.c, $(SRCSS))
OBJS=$(SRCS:.c=.o)

cc: $(OBJS)
		$(CC) -o cc $(OBJS) $(LDFLAGS)

$(OBJS): cc.h

test: cc
	./test_f.sh

clean:
	rm -f cc *.o *~ tmp* *.log

db:
	gdb cc
	
.PHONY: test clean
