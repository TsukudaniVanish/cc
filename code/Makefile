CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
SRCSNOHELPER=$(filter-out helper_functions.c, $(SRCS))
BACKENDSRCS=$(wildcard ./backend/*.c)
BACKENDOBJS=$(BACKENDSRCS:.c=.o)
OBJS=$(SRCSNOHELPER:.c=.o)

cc: $(OBJS) $(BACKENDOBJS)
		$(CC) -o cc $(OBJS) $(BACKENDOBJS) $(LDFLAGS)

$(OBJS): cc.h
$(BACKENDOBJS): ./backend/file.h

test: cc
	./cc -f ./test/test.c > tmp.s
	cc -g -o tmp tmp.s
	./tmp
utest: cc
	./cc -f ./test/test_2.c > tmp.s
	cc -g -o tmp tmp.s
	./tmp

etest: cc
	./test/test.sh

clean:
	rm -f cc *.o *~ tmp* *.log ./backend/*.o

db:
	gdb cc
	
.PHONY: test clean
