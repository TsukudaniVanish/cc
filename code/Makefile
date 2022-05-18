CFLAGS=-std=c11 -g -static
SRC=$(wildcard *.c)
SRCSS=$(filter-out test.c, $(SRC))
SRCSSS=$(filter-out helper_functions.c, $(SRCSS))
SRCS=$(filter-out test_2.c, $(SRCSSS))
OBJS=$(SRCS:.c=.o)
HELPER=helper_functions.o

cc: $(OBJS)
		$(CC) -o cc $(OBJS) $(LDFLAGS)

$(OBJS): cc.h

test: cc $(HELPER)
	./cc -f ./test/test.c > tmp.s
	cc -g -o tmp tmp.s $(HELPER)
	./tmp
utest: cc $(HELPER)
	./cc -f ./test/test_2.c > tmp.s
	cc -g -o tmp tmp.s $(HELPER)
	./tmp

etest: cc $(HELPER)
	./test/test.sh

clean:
	rm -f cc *.o *~ tmp* *.log

db:
	gdb cc
	
.PHONY: test clean
