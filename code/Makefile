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

unit_test: cc 
	./cc -T

all_test: cc 
	sh ./test/test_all.sh 

clean:
	rm -f cc *.o *~ tmp* *.log ./backend/*.o

db:
	gdb cc
	
.PHONY: test clean utest etest unit_test all_test
