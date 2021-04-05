CC = gcc
CFLAGS = -Wall -pedantic -g -fsanitize=address -std=c99
CLIBS = -pthread -lrt 
RM = rm -f

SOURCES = $(wildcard *.c)
BINS = $(SOURCES:.c=.out)

CPPANS = $(SOURCES:.c=.cppout)	

TESTF = Archive/jnh/*

all: $(BINS)

%.out: %.c
	$(CC) $(CFLAGS) $(CLIBS) $^ -o $@ 

clean: clean_test
	$(RM) $(BINS)

clean_test:
	$(RM) $(CPPANS) *.valout report.tasks

test: clean $(CPPANS)
	valgrind ./solve.out $(TESTF) 2> master.valout| valgrind ./vista.out 2> view.valout > /dev/null; echo ""

%.cppout: %.c
	cppcheck --quiet --enable=all --force --inconclusive  $^ 2> $@

.PHONY: all clean test clean_test