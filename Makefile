OBJS=$(wildcard *.o)
DEPENDS=$(wildcard *.d)

CFLAGS=-Wall -Wextra -pedantic -g -MD -std=c99

.PHONY: clean

disassembler: clean
	cc -g disassembler.c -o dis $(CFLAGS)

clean:
	$(RM) dis $(OBJS) $(DEPENDS)
