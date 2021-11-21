CFLAGS=-std=c11 -g -static

cc: cc.c

test: cc
	./test.sh

clean:
	rm -f cc *.o *~ tmp* *.log

db:
	gdb cc

.PHONY: test clean
